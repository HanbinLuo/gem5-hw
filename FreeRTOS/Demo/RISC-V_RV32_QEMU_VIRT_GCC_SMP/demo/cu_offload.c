/* cu_offload.c */

#include "cu_offload.h"

#include "dag_runtime.h"
#include "uart16550.h"

/* 假设 DRAM 起始 0x80000000，这里随便两个 buffer */
#define SRC_BUF 0x90001000u
#define DST_BUF 0x90002000u

#define CU_BASE 0x10009000UL
#define CU_OP_A (CU_BASE + 0x00)
#define CU_OP_B (CU_BASE + 0x10)
#define CU_RESULT (CU_BASE + 0x20)
#define CU_LENGTH (CU_BASE + 0x30)
#define CU_CONFIG (CU_BASE + 0x31)
#define CU_STATUS (CU_BASE + 0x32)

// 简单的 MMIO 读写（8-bit）
static inline void mmio_write8(uintptr_t addr, uint8_t v) {
  volatile uint8_t* p = (volatile uint8_t*)addr;
  *p = v;
}

static inline uint8_t mmio_read8(uintptr_t addr) {
  volatile uint8_t* p = (volatile uint8_t*)addr;
  return *p;
}

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

/* 假设 DRAM 起始 0x80000000，这里随便两个 buffer */
#define SRC_BUF 0x90001000u
#define DST_BUF 0x90002000u

static void dma_memcpy(void* dst, const void* src, uint32_t len) {
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
}

void vCuHwStartJob(uint32_t cu_id, uint32_t job_id) {
  uint8_t* src = (uint8_t*)SRC_BUF;
  uint8_t* dst = (uint8_t*)DST_BUF;
  const uint32_t len = 256;

  /* 初始化 src，dst 先清零 */
  for (uint32_t i = 0; i < len; ++i) {
    src[i] = (uint8_t)(i & 0xFF);
    dst[i] = 0;
  }

  uint8_t cfg = 0;  // 0 = add, 1 = sub

  // 写入操作数
  dma_memcpy((void*)CU_OP_A, src, 16);
  dma_memcpy((void*)CU_OP_B, src + 16, 16);
  // 写入计算长度
  mmio_write8(CU_LENGTH, 16);
  // 触发计算（写 config）
  //   unsigned long long t0 = rdcycle64();
  mmio_write8(CU_CONFIG, cfg);

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

// __attribute__((weak)) void vCuHwStartJob(uint32_t cu_id, uint32_t job_id)
// {
//     (void)cu_id;
//     (void)job_id;
// }

void vCuInit(void) {
  for (uint32_t i = 0; i < CU_MAX_COUNT; ++i) {
    gCuSlots[i].waiter = NULL;
    gCuSlots[i].dag_node = NULL;
    gCuSlots[i].job_id = 0;
  }
}

void vCuSubmitJobAndWait(uint32_t cu_id, uint32_t job_id) {
  configASSERT(cu_id < CU_MAX_COUNT);

  /* TODO:使能对应CU IRQ */

  TaskHandle_t self = xTaskGetCurrentTaskHandle();

  taskENTER_CRITICAL();
  /* 当前设计：每个 CU 同一时间只允许 1 个 inflight 任务 */
  configASSERT(gCuSlots[cu_id].waiter == NULL);
  configASSERT(gCuSlots[cu_id].dag_node == NULL);
  gCuSlots[cu_id].waiter = self;
  gCuSlots[cu_id].job_id = job_id;
  taskEXIT_CRITICAL();

  /* 真正启动 CU + DMA */
  vCuHwStartJob(cu_id, job_id);

  /* 阻塞等待 PLIC 中断唤醒（不 busy-wait） */
  (void)ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

  /* ISR 负责清理 slot，这里做一下防御性检查 */
  configASSERT(gCuSlots[cu_id].waiter == NULL);
  configASSERT(gCuSlots[cu_id].dag_node == NULL);
}

void vCuSubmitDagJob(uint32_t cu_id, uint32_t job_id, struct DagNode* node) {
  configASSERT(cu_id < CU_MAX_COUNT);
  configASSERT(node != NULL);

  /* TODO:使能对应CU IRQ */

  taskENTER_CRITICAL();
  configASSERT(gCuSlots[cu_id].waiter == NULL);
  configASSERT(gCuSlots[cu_id].dag_node == NULL);
  gCuSlots[cu_id].dag_node = node;
  gCuSlots[cu_id].job_id = job_id;
  taskEXIT_CRITICAL();

  vCuHwStartJob(cu_id, job_id);
}

void vCuHandleIsr(uint32_t cu_id, BaseType_t* pxHigherPriorityTaskWoken) {
  configASSERT(cu_id < CU_MAX_COUNT);

  TaskHandle_t waiter = gCuSlots[cu_id].waiter;
  struct DagNode* dag_node = gCuSlots[cu_id].dag_node;

  LOGF("Node %s handled CU %d ISR on core %d\n", gCuSlots[cu_id].dag_node->name,
       cu_id, (uint32_t)portGET_CORE_ID());

  /* TEST: 读取结果并打印 */
  uint8_t* src = (uint8_t*)SRC_BUF;
  uint8_t* dst = (uint8_t*)DST_BUF;
  dma_memcpy(dst, (void*)CU_RESULT, 16);
  for (int i = 0; i < 16; i++) {
    LOGF("[%d] %u + %u =%u\n", i, src[i], src[i + 16], dst[i]);
  }

  gCuSlots[cu_id].waiter = NULL;
  gCuSlots[cu_id].dag_node = NULL;
  gCuSlots[cu_id].job_id = 0;

  if (waiter != NULL) {
    vTaskNotifyGiveFromISR(waiter, pxHigherPriorityTaskWoken);
  }

  if (dag_node != NULL) {
    vDagNotifyAsyncDoneFromISR(dag_node, pxHigherPriorityTaskWoken);
  }
}
