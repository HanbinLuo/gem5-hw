/* dag_runtime.c */

#include "dag_runtime.h"

#include <string.h>

#include "plic_handler.h"
#include "portmacro.h"
#include "uart16550.h"
#include "dag_debug.h"

#define DAG_MAX_WORKERS configNUMBER_OF_CORES

typedef struct {
  DagRuntimeConfig cfg;
  QueueHandle_t readyQ;
  QueueHandle_t asyncDoneQ;
  TaskHandle_t workers[DAG_MAX_WORKERS];
  TaskHandle_t asyncTask;
  uint32_t tempBytes;
  uint32_t tempPeakBytes;
} DagRuntimeState_t;

static DagRuntimeState_t gDag;
/* Worker count override (defined here so it's linked into the demo binary) */
UBaseType_t gDagWorkerCountOverride = 0u;

void vDagSetWorkerCountOverride(UBaseType_t wc) {
  taskENTER_CRITICAL();
  gDagWorkerCountOverride = wc;
  taskEXIT_CRITICAL();
}

/* For measuring DAG completion */
static uint32_t gDagTotalNodes = 0U;
static uint32_t gDagFinishedNodes = 0U;
static unsigned long long gDagStartCycles = 0ULL;
static unsigned long long gDagEndCycles = 0ULL;

static inline unsigned long long rdcycle64_local(void) {
  unsigned long long v;
  __asm__ volatile("rdcycle %0" : "=r"(v));
  return v;
}

void vDagSetTotalNodes(uint32_t total) {
  taskENTER_CRITICAL();
  gDagTotalNodes = total;
  gDagFinishedNodes = 0U;
  gDagStartCycles = rdcycle64_local();
  gDagEndCycles = 0ULL;
  taskEXIT_CRITICAL();
  /* 移到临界区外避免死锁 */
  LOGF("DAG total nodes set=%u\n", total);
}

static inline uint32_t dag_atomic_dec(uint32_t* pValue) {
  uint32_t newVal;

  taskENTER_CRITICAL();
  (*pValue)--;
  newVal = *pValue;
  taskEXIT_CRITICAL();

  return newVal;
}

static void vDagTempAlloc(uint32_t sizeBytes) {
  uint32_t currentBytes, peakBytes;
  
  taskENTER_CRITICAL();
  gDag.tempBytes += sizeBytes;
  if (gDag.tempBytes > gDag.tempPeakBytes) {
    gDag.tempPeakBytes = gDag.tempBytes;
  }
  currentBytes = gDag.tempBytes;
  peakBytes = gDag.tempPeakBytes;
  taskEXIT_CRITICAL();
  
  #ifdef DEBUG_LOGF
  LOGF("TempMem alloc %u bytes, current=%u, peak=%u\n", sizeBytes,
       currentBytes, peakBytes);
  #endif
}

static void vDagTempFree(uint32_t sizeBytes) {
  uint32_t currentBytes, peakBytes;
  
  taskENTER_CRITICAL();
  if (gDag.tempBytes >= sizeBytes) {
    gDag.tempBytes -= sizeBytes;
  } else {
    gDag.tempBytes = 0U;
  }
  currentBytes = gDag.tempBytes;
  peakBytes = gDag.tempPeakBytes;
  taskEXIT_CRITICAL();
  
  #ifdef DEBUG_LOGF
  LOGF("TempMem free %u bytes, current=%u, peak=%u\n", sizeBytes,
       currentBytes, peakBytes);
  #endif
}

static void vDagProduceOutputs(DagNode* node) {
  for (uint32_t i = 0; i < node->numOutputs; i++) {
    DagData* data = node->outputs[i];
    if (data == NULL) {
      continue;
    }
    data->remaining_consumers = data->consumers;
    if (data->sizeBytes == 0U || data->consumers == 0U) {
      continue;
    }
    if (data->buffer == NULL) {
      data->buffer = pvPortMalloc(data->sizeBytes);
      configASSERT(data->buffer != NULL);
      vDagTempAlloc(data->sizeBytes);
    }
  }
}

void vDagPrepareAsyncOutputs(DagNode* node) {
  dag_debug_write8(DAG_DEBUG_CONFIG, 2u); /* 2: prepare async outputs */
  for (uint32_t i = 0; i < node->numOutputs; i++) {
    DagData* data = node->outputs[i];
    if (data == NULL) {
      continue;
    }
    if (data->sizeBytes == 0U || data->consumers == 0U) {
      continue;
    }
    if (data->buffer == NULL) {
      data->buffer = pvPortMalloc(data->sizeBytes);
      configASSERT(data->buffer != NULL);
      vDagTempAlloc(data->sizeBytes);
    }
  }
}

static void vDagReleaseInputs(DagNode* node) {
  for (uint32_t i = 0; i < node->numInputs; i++) {
    DagData* data = node->inputs[i];
    if (data == NULL || data->buffer == NULL) {
      continue;
    }
    if (data->remaining_consumers == 0U) {
      continue;
    }
    uint32_t remaining = dag_atomic_dec(&data->remaining_consumers);
    if (remaining == 0U) {
      vPortFree(data->buffer);
      data->buffer = NULL;
      vDagTempFree(data->sizeBytes);
    }
  }
}

static void vDagEnqueueSuccessors(DagNode* node) {
  for (uint32_t i = 0; i < node->numSuccessors; i++) {
    DagNode* succ = node->successors[i];
    uint32_t newIndegree = dag_atomic_dec(&succ->indegree);

    if (newIndegree == 0U) {
      DagNode* ready = succ;
      configASSERT(xQueueSend(gDag.readyQ, &ready, 0) == pdPASS);
    }
  }
}

static void vDagFinalizeNode(DagNode* node) {
  dag_debug_write8(DAG_DEBUG_CONFIG, 12u); /* 12: finalize node */
  vDagProduceOutputs(node);
  vDagReleaseInputs(node);
  vDagEnqueueSuccessors(node);

  /* Track completion for timing */
  if (gDagTotalNodes != 0U) {
    uint32_t finished;
    uint32_t diff32 = 0;
    uint8_t isFinished = 0;
    
    taskENTER_CRITICAL();
    gDagFinishedNodes++;
    finished = gDagFinishedNodes;
    if (finished == gDagTotalNodes) {
      gDagEndCycles = rdcycle64_local();
      unsigned long long diff = gDagEndCycles - gDagStartCycles;
      diff32 = (uint32_t)(diff & 0xFFFFFFFFULL);
      isFinished = 1;
    }
    taskEXIT_CRITICAL();
    
    if (isFinished) {
      LOGF("DAG finished: cycles=%u\n", diff32);
    }
  }
}

static void vDagWorkerTask(void* pvParam) {
  (void)pvParam;
  DagNode* node;
#ifdef DEBUG_LOGF
  LOGF("Dag worker started on core %d\n", (uint32_t)portGET_CORE_ID());
#endif
  /* 1) 使能机器外部中断 */
  vEnableMachineExternalInterrupts();

  for (;;) {
    if (xQueueReceive(gDag.readyQ, &node, portMAX_DELAY) == pdTRUE) {
      dag_debug_write8(DAG_DEBUG_CONFIG, 1u); /* 1: node run start */
      node->run(node->arg);
      dag_debug_write8(DAG_DEBUG_CONFIG, 13u); /* 13: node run end */

      if (node->mode == DAG_NODE_SYNC) {
        vDagFinalizeNode(node);
      }
    }
  }
}

static void vDagAsyncCompleteTask(void* pvParam) {
  (void)pvParam;
  DagNode* node;

  for (;;) {
    if (xQueueReceive(gDag.asyncDoneQ, &node, portMAX_DELAY) == pdTRUE) {
      // vDemoLogString(node->name);
      // vDemoLogDecimal("CU node done on core",
      //                 (uint32_t)portGET_CORE_ID());
      #ifdef DEBUG_LOGF
      LOGF("Node %s done on core %d\n", node->name,
           (uint32_t)portGET_CORE_ID());
      #endif
      dag_debug_write8(DAG_DEBUG_CONFIG, 11u); /* 11: async completion dequeued */
      vDagFinalizeNode(node);
    }
  }
}

void vDagRuntimeStart(const DagRuntimeConfig* cfg) {
  configASSERT(cfg != NULL);
  configASSERT(cfg->workerCount <= DAG_MAX_WORKERS);

  memset(&gDag, 0, sizeof(gDag));
  gDag.cfg = *cfg;

  gDag.readyQ = xQueueCreate(cfg->readyQueueLen, sizeof(DagNode*));
  configASSERT(gDag.readyQ != NULL);

  gDag.asyncDoneQ = xQueueCreate(cfg->asyncDoneQueueLen, sizeof(DagNode*));
  configASSERT(gDag.asyncDoneQ != NULL);

  for (UBaseType_t i = 0; i < cfg->workerCount; i++) {
    BaseType_t rc =
        xTaskCreate(vDagWorkerTask, "DagWorker", cfg->workerStackWords, NULL,
                    cfg->workerPriority, &gDag.workers[i]);
    configASSERT(rc == pdPASS);

#if (configUSE_CORE_AFFINITY == 1)
    if (cfg->useCoreAffinity) {
      vTaskCoreAffinitySet(gDag.workers[i], (1U << i));
    }
#endif
  }

  BaseType_t rc =
      xTaskCreate(vDagAsyncCompleteTask, "DagAsync", cfg->asyncStackWords, NULL,
                  cfg->asyncPriority, &gDag.asyncTask);
  configASSERT(rc == pdPASS);
}

void vDagSubmitReadyNode(DagNode* node) {
  configASSERT(node != NULL);
  dag_debug_write8(DAG_DEBUG_CONFIG, 0u); /* 0: node queued */
  xQueueSend(gDag.readyQ, &node, portMAX_DELAY);
}

void vDagNotifyAsyncDoneFromISR(DagNode* node,
                                BaseType_t* pxHigherPriorityTaskWoken) {
  configASSERT(node != NULL);
  node->async_pending = 0U;
  configASSERT(xQueueSendFromISR(gDag.asyncDoneQ, &node,
                                 pxHigherPriorityTaskWoken) == pdPASS);
}

uint32_t ulDagGetTempBytes(void) {
  uint32_t value;
  taskENTER_CRITICAL();
  value = gDag.tempBytes;
  taskEXIT_CRITICAL();
  return value;
}

uint32_t ulDagGetTempPeakBytes(void) {
  uint32_t value;
  taskENTER_CRITICAL();
  value = gDag.tempPeakBytes;
  taskEXIT_CRITICAL();
  return value;
}
