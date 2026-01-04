#include "plic_handler.h"

#include <stdint.h>

#include "FreeRTOS.h"
#include "cu_offload.h"
#include "task.h"

/* DAG Debug 寄存器定义 */
#define DAG_DEBUG_BASE    0x20100000u
#define DAG_DEBUG_CU_ID   (DAG_DEBUG_BASE + 0x00u)  /* uint32_t cu_id_reg */
#define DAG_DEBUG_JOB_ID  (DAG_DEBUG_BASE + 0x04u)  /* uint32_t job_id_reg */
#define DAG_DEBUG_CONFIG  (DAG_DEBUG_BASE + 0x08u)  /* uint8_t config */

static inline void dag_debug_write32(uint32_t addr, uint32_t val) {
  *(volatile uint32_t*)addr = val;
}

static inline void dag_debug_write8(uint32_t addr, uint8_t val) {
  *(volatile uint8_t*)addr = val;
}

static inline void plic_mmio_write32(uint32_t addr, uint32_t val) {
  *(volatile uint32_t*)addr = val;
}

static inline uint32_t plic_mmio_read32(uint32_t addr) {
  return *(volatile uint32_t*)addr;
}

static inline uint32_t plic_priority_addr(uint32_t source) {
  return PLIC_BASE + 4u * source;
}

/* 修改使能寄存器地址计算，增加根据 irqID 的偏移 */
static inline uint32_t plic_enable_addr(uint32_t context, uint32_t irqID) {
  // 每个寄存器管 32 个中断，所以偏移是 (irqID / 32) * 4 字节
  return PLIC_BASE + 0x002000u + (context * 0x80u) + ((irqID / 32u) * 4u);
}

static inline uint32_t plic_threshold_addr(uint32_t context) {
  return PLIC_BASE + 0x200000u + (context * 0x1000u);
}

static inline uint32_t plic_claim_addr(uint32_t context) {
  return PLIC_BASE + 0x200004u + (context * 0x1000u);
}

/* PLIC 初始化：设置优先级、使能 CU0 IRQ、阈值为 0 */
void vPlicInit(uint32_t hartID, uint32_t irqID) {
  plic_mmio_write32(plic_priority_addr(irqID), 1u);

  // 计算该中断号在对应 32 位寄存器中的具体哪一位
  uint32_t addr = plic_enable_addr(hartID, irqID);
  uint32_t bit = irqID % 32u;

  uint32_t enables = plic_mmio_read32(addr);
  enables |= (1u << bit);
  plic_mmio_write32(addr, enables);

  plic_mmio_write32(plic_threshold_addr(hartID), 0u);
}

void vPlicDeinit(uint32_t hartID, uint32_t irqID) {
  uint32_t addr = plic_enable_addr(hartID, irqID);
  uint32_t bit = irqID % 32u;

  uint32_t enables = plic_mmio_read32(addr);
  enables &= ~(1u << bit);
  plic_mmio_write32(addr, enables);
}

/*
 * 外部中断处理函数，由 RISC-V FreeRTOS 端口在
 * freertos_risc_v_application_interrupt_handler() 里调用。
 *
 * 返回值用于决定是否在中断尾部调用 portYIELD_FROM_ISR()。
 */
BaseType_t xPortHandleExternalInterrupt(uint32_t ulMcause, uint32_t ulMepc) {
  (void)ulMcause;
  (void)ulMepc;

  //   uint32_t claim = plic_mmio_read32(plic_claim_addr(PLIC_CONTEXT));
  uint32_t claim = plic_mmio_read32(plic_claim_addr(portGET_CORE_ID()));
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  if (claim != 0u) {
    // plic_mmio_write32(plic_claim_addr(PLIC_CONTEXT), claim);
    plic_mmio_write32(plic_claim_addr(portGET_CORE_ID()), claim);
  }

  /* 去使能对应的CU IRQ */
  for (int i = 0; i < configNUMBER_OF_CORES; ++i) {
    vPlicDeinit(i, claim);
  }

  /*
    * 处理 CU IRQ 事件
   */
  if (claim >= PLIC_IRQ_CU0 && claim <= PLIC_IRQ_CU_LAST) {
    /* 将 PLIC claim 映射为 cu_id（假定 CU IRQ 号连续分配） */
    uint32_t cu_id = claim - PLIC_IRQ_CU0;
    
    /* DAG Debug: 写入cu_id和job_id，config=1表示开始 */
    dag_debug_write32(DAG_DEBUG_CU_ID, cu_id);
    dag_debug_write32(DAG_DEBUG_JOB_ID, 0);  /* job_id暂用cu_id，实际可从gCuSlots获取 */
    dag_debug_write8(DAG_DEBUG_CONFIG, 1u);  /* 1 = 函数开始 */
    
    /* 调用 CU/DAG 桥接层处理该 CU 的完成事件 */
    vCuHandleIsr(cu_id, &xHigherPriorityTaskWoken);
    
    /* DAG Debug: config=2表示结束 */
    dag_debug_write8(DAG_DEBUG_CONFIG, 2u);  /* 2 = 函数结束 */
  }
  return xHigherPriorityTaskWoken;
}

/* 在需要接收 PLIC 中断的 hart 上调用 */
void vEnableMachineExternalInterrupts(void) {
  const uint32_t mie_meie = (1u << 11);

  __asm__ volatile("csrs mie, %0" ::"r"(mie_meie));
}