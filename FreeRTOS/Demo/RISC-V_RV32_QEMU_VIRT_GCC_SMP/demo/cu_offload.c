/* cu_offload.c */

#include "cu_offload.h"

#include <string.h>

#include "dag_runtime.h"
#include "plic_handler.h"
#include "uart16550.h"

// 读 RISC-V mcycle（裸机运行在 machine/privileged 模式），用于测周期数
static inline unsigned long long rdcycle64(void) {
  unsigned long long v;
  // 如果目标支持 rdcycle:
  __asm__ volatile("rdcycle %0" : "=r"(v));
  return v;
}

#define DMA_BASE 0x50000000u

typedef struct {
  volatile uint32_t SRC;     // 0x00
  volatile uint32_t DST;     // 0x04
  volatile uint32_t LEN;     // 0x08
  volatile uint32_t CTRL;    // 0x0C
  volatile uint32_t STATUS;  // 0x10
} simple_dma_regs_t;

#define DMA_CTRL_START (1u << 0)
#define DMA_STATUS_DONE (1u << 0)

static simple_dma_regs_t* const dma = (simple_dma_regs_t*)DMA_BASE;

static uint32_t lcg_next(uint32_t v) { return v * 1664525u + 1013904223u; }

static BaseType_t vCuIsHwIdle(uint32_t id) {
  uint8_t busy = mmio_read8(cu_base(id) + CU_BUSY_OFF);
  /* busy register bit0 == 0 表示空闲 */
  return (busy & 0x1u) == 0u;
}

static void dma_memcpy(void* dst, const void* src, uint32_t len) {
  if (len == 0U) {
    return;
  }

  /* DMA 控制器是全局共享资源，必须加锁保护 */
  UBaseType_t uxSavedStatus = taskENTER_CRITICAL_FROM_ISR();

  /* 清 DONE 标志 */
  dma->STATUS = DMA_STATUS_DONE;

  dma->SRC = (uint32_t)src;
  dma->DST = (uint32_t)dst;
  dma->LEN = len;
  dma->CTRL = DMA_CTRL_START;

  /* 简单轮询等待 DMA 完成 */
  while ((dma->STATUS & DMA_STATUS_DONE) == 0) {
    /* busy wait */
  }

  taskEXIT_CRITICAL_FROM_ISR(uxSavedStatus);
}

static uintptr_t cu_output_src_addr(uint32_t cu_id, uint32_t index) {
  return (uintptr_t)(cu_base(cu_id) + CU_OUTPUT_ADDR0_OFF +
                     index * CU_OUTPUT_STRIDE);
}

void vCuHwStartJob(uint32_t cu_id, uint32_t job_id, const CuSlot_t* job_info) {
  const CuSlot_t* job_slot = job_info;
  const uint32_t numInputs = (job_slot != NULL) ? job_slot->job.numInputs : 0U;
  const uint32_t numOutputs =
      (job_slot != NULL) ? job_slot->job.numOutputs : 0U;

  (void)job_id;

  /* 搬移数据到 CU */
  for (uint32_t i = 0; i < numInputs; i++) {
    dma_memcpy((void*)cu_base(cu_id) + CU_INPUT_ADDR0_OFF + i * CU_INPUT_STRIDE,
               (const void*)job_slot->job.inputAddrs[i],
               job_slot->job.inputSizes[i]);
  }

  if (job_slot != NULL) {
    mmio_write32(cu_base(cu_id) + CU_JOB_OFF, job_id);
  }

  if (job_slot != NULL) {
    uint32_t taskSize = 0;
    for (int i = 0; i < numInputs; i++) {
      taskSize += job_slot->job.inputSizes[i];
    }
    mmio_write32(cu_base(cu_id) + CU_SIZE_OFF, taskSize);
  }

  if (job_slot != NULL) {
    mmio_write32(cu_base(cu_id) + CU_DELAY_OFF, job_slot->job.computeDelayMs);
  }

  mmio_write8(cu_base(cu_id) + CU_CONFIG_OFF, 1U);

  //   // 轮询 status bit0 == 1
  //   while ((mmio_read8(CU_STATUS) & 0x1) == 0) {
  //   }

  //   unsigned long long t1 = rdcycle64();

  //   dma_memcpy(dst, (void*)CU_RESULT, 16);

  //   unsigned long long cycles = t1 - t0;
  //   for (int i = 0; i < 16; i++) {
  //     LOGF("[%d] %u + %u =%u\n", i, src[i], src[i + 16], dst[i]);
  //   }
  //   LOGF("Compute cycles: %d \n", (int)cycles);
}

static CuSlot_t gCuSlots[CU_MAX_COUNT];

// __attribute__((weak)) void vCuHwStartJob(uint32_t cu_id, uint32_t job_id,
//                                          const void* job_info)
// {
//     (void)cu_id;
//     (void)job_id;
//     (void)job_info;
// }

void vCuInit(void) {
  for (uint32_t i = 0; i < CU_MAX_COUNT; ++i) {
    gCuSlots[i].waiter = NULL;
    gCuSlots[i].dag_node = NULL;
    gCuSlots[i].job_id = 0;
    memset(&gCuSlots[i].job, 0, sizeof(gCuSlots[i].job));
  }
}

uint32_t vCuSubmitJobAndWait(uint32_t cu_id, uint32_t job_id) {
  static uint32_t s_rand = 0x13579bdfu;
  TaskHandle_t self = xTaskGetCurrentTaskHandle();

  for (;;) {
    taskENTER_CRITICAL();
    if (cu_id == CU_ANY_ID) {
      uint32_t start = s_rand % CU_MAX_COUNT;
      for (uint32_t i = 0; i < CU_MAX_COUNT; i++) {
        uint32_t id = (start + i) % CU_MAX_COUNT;
        if (vCuIsHwIdle(id) && gCuSlots[id].waiter == NULL &&
            gCuSlots[id].dag_node == NULL) {
          cu_id = id;
          break;
        }
      }
      s_rand = lcg_next(s_rand);
    }

    if (cu_id < CU_MAX_COUNT && vCuIsHwIdle(cu_id) &&
        gCuSlots[cu_id].waiter == NULL && gCuSlots[cu_id].dag_node == NULL) {
      gCuSlots[cu_id].waiter = self;
      gCuSlots[cu_id].job_id = job_id;
      taskEXIT_CRITICAL();
      break;
    }
    taskEXIT_CRITICAL();
    vTaskDelay(pdMS_TO_TICKS(1));
  }

  /* 使能对应CU IRQ */
  for (int i = 0; i < configNUMBER_OF_CORES; ++i) {
    vPlicInit(i, PLIC_IRQ_CU0 + cu_id);
  }

  /* 真正启动 CU + DMA */
  vCuHwStartJob(cu_id, job_id, NULL);

  /* 阻塞等待 PLIC 中断唤醒（不 busy-wait） */
  (void)ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

  /* ISR 负责清理 slot，这里做一下防御性检查 */
  configASSERT(gCuSlots[cu_id].waiter == NULL);
  configASSERT(gCuSlots[cu_id].dag_node == NULL);

  return cu_id;
}

uint32_t vCuSubmitDagJob(uint32_t cu_id, uint32_t job_id,
                         struct DagNode* node) {
  configASSERT(node != NULL);

  vDagPrepareAsyncOutputs(node);

  static uint32_t s_rand = 0x2468ace0u;

  for (;;) {
    taskENTER_CRITICAL();
    if (cu_id == CU_ANY_ID) {
      uint32_t start = s_rand % CU_MAX_COUNT;
      for (uint32_t i = 0; i < CU_MAX_COUNT; i++) {
        uint32_t id = (start + i) % CU_MAX_COUNT;
        if (vCuIsHwIdle(id) && gCuSlots[id].waiter == NULL &&
            gCuSlots[id].dag_node == NULL) {
          cu_id = id;
          break;
        }
      }
      s_rand = lcg_next(s_rand);
    }

    if (cu_id < CU_MAX_COUNT && vCuIsHwIdle(cu_id) &&
        gCuSlots[cu_id].waiter == NULL && gCuSlots[cu_id].dag_node == NULL) {
      gCuSlots[cu_id].dag_node = node;
      gCuSlots[cu_id].job_id = job_id;
      memset(&gCuSlots[cu_id].job, 0, sizeof(gCuSlots[cu_id].job));
      gCuSlots[cu_id].job.computeDelayMs = node->computeDelayMs;
      gCuSlots[cu_id].job.storageSizeBytes = node->storageSizeBytes;
      taskEXIT_CRITICAL();
      break;
    }
    taskEXIT_CRITICAL();
    vTaskDelay(pdMS_TO_TICKS(1));
  }

  /* 使能对应CU IRQ */
  for (int i = 0; i < configNUMBER_OF_CORES; ++i) {
    vPlicInit(i, PLIC_IRQ_CU0 + cu_id);
  }

  for (uint32_t i = 0; i < node->numInputs && i < CU_MAX_IO; i++) {
    DagData* data = node->inputs[i];
    if (data == NULL || data->buffer == NULL) {
      continue;
    }
    gCuSlots[cu_id].job.inputAddrs[gCuSlots[cu_id].job.numInputs] =
        (uintptr_t)data->buffer;
    gCuSlots[cu_id].job.inputSizes[gCuSlots[cu_id].job.numInputs] =
        data->sizeBytes;
    gCuSlots[cu_id].job.numInputs++;
  }

  for (uint32_t i = 0; i < node->numOutputs && i < CU_MAX_IO; i++) {
    DagData* data = node->outputs[i];
    if (data == NULL || data->buffer == NULL) {
      continue;
    }
    gCuSlots[cu_id].job.outputAddrs[gCuSlots[cu_id].job.numOutputs] =
        (uintptr_t)data->buffer;
    gCuSlots[cu_id].job.outputSizes[gCuSlots[cu_id].job.numOutputs] =
        data->sizeBytes;
    gCuSlots[cu_id].job.numOutputs++;
  }

  vCuHwStartJob(cu_id, job_id, &gCuSlots[cu_id]);
  return cu_id;
}

void vCuHandleIsr(uint32_t cu_id, BaseType_t* pxHigherPriorityTaskWoken) {
  configASSERT(cu_id < CU_MAX_COUNT);

  TaskHandle_t waiter = gCuSlots[cu_id].waiter;
  struct DagNode* dag_node = gCuSlots[cu_id].dag_node;

  if (dag_node != NULL) {
    LOGF("Node %s handled CU %d ISR on core %d\n", dag_node->name, cu_id,
         (uint32_t)portGET_CORE_ID());
  }

  /* 回收输出：从 CU 侧地址 DMA 到输出 buffer */
  for (uint32_t i = 0; i < gCuSlots[cu_id].job.numOutputs; i++) {
    void* dst = (void*)gCuSlots[cu_id].job.outputAddrs[i];
    uint32_t len = gCuSlots[cu_id].job.outputSizes[i];
    if (dst == NULL || len == 0U) {
      continue;
    }
    dma_memcpy(dst, (void*)cu_output_src_addr(cu_id, i), len);
  }

  UBaseType_t uxSavedStatus = taskENTER_CRITICAL_FROM_ISR();
  gCuSlots[cu_id].waiter = NULL;
  gCuSlots[cu_id].dag_node = NULL;
  gCuSlots[cu_id].job_id = 0;
  memset(&gCuSlots[cu_id].job, 0, sizeof(gCuSlots[cu_id].job));
  taskEXIT_CRITICAL_FROM_ISR(uxSavedStatus);

  if (waiter != NULL) {
    vTaskNotifyGiveFromISR(waiter, pxHigherPriorityTaskWoken);
  }

  if (dag_node != NULL) {
    vDagNotifyAsyncDoneFromISR(dag_node, pxHigherPriorityTaskWoken);
  }
}
