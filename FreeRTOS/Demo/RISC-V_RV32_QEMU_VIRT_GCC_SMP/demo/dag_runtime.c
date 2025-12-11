/* dag_runtime.c */

#include "dag_runtime.h"

#include <string.h>

#include "portmacro.h"
#include "uart16550.h"

#define DAG_MAX_WORKERS configNUMBER_OF_CORES

typedef struct {
  DagRuntimeConfig cfg;
  QueueHandle_t readyQ;
  QueueHandle_t asyncDoneQ;
  TaskHandle_t workers[DAG_MAX_WORKERS];
  TaskHandle_t asyncTask;
} DagRuntimeState_t;

static DagRuntimeState_t gDag;

static inline uint32_t dag_atomic_dec(uint32_t* pValue) {
  uint32_t newVal;

  taskENTER_CRITICAL();
  (*pValue)--;
  newVal = *pValue;
  taskEXIT_CRITICAL();

  return newVal;
}

static void vDagEnqueueSuccessors(DagNode* node) {
  for (uint32_t i = 0; i < node->numSuccessors; i++) {
    DagNode* succ = node->successors[i];
    uint32_t newIndegree = dag_atomic_dec(&succ->indegree);

    if (newIndegree == 0U) {
      DagNode* ready = succ;
      xQueueSend(gDag.readyQ, &ready, portMAX_DELAY);
    }
  }
}

static void vDagWorkerTask(void* pvParam) {
  (void)pvParam;
  DagNode* node;

  for (;;) {
    if (xQueueReceive(gDag.readyQ, &node, portMAX_DELAY) == pdTRUE) {
      node->run(node->arg);

      if (node->mode == DAG_NODE_SYNC) {
        vDagEnqueueSuccessors(node);
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
      LOGF("Node %s done on core %d\n", node->name,
           (uint32_t)portGET_CORE_ID());
      vDagEnqueueSuccessors(node);
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
  xQueueSend(gDag.readyQ, &node, portMAX_DELAY);
}

void vDagNotifyAsyncDoneFromISR(DagNode* node,
                                BaseType_t* pxHigherPriorityTaskWoken) {
  configASSERT(node != NULL);
  node->async_pending = 0U;
  xQueueSendFromISR(gDag.asyncDoneQ, &node, pxHigherPriorityTaskWoken);
}
