#include "plic_handler.h"

#include <stdint.h>

#include "FreeRTOS.h"
#include "cu_offload.h"
#include "task.h"

static inline void plic_mmio_write32(uint32_t addr, uint32_t val) {
  *(volatile uint32_t*)addr = val;
}

static inline uint32_t plic_mmio_read32(uint32_t addr) {
  return *(volatile uint32_t*)addr;
}

static inline uint32_t plic_priority_addr(uint32_t source) {
  return PLIC_BASE + 4u * source;
}

static inline uint32_t plic_enable_addr(uint32_t context) {
  return PLIC_BASE + 0x002000u + (context * 0x80u);
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

  uint32_t enables = plic_mmio_read32(plic_enable_addr(hartID));
  enables |= (1u << irqID);
  plic_mmio_write32(plic_enable_addr(hartID), enables);

  plic_mmio_write32(plic_threshold_addr(hartID), 0u);
}

void vPlicDeinit(uint32_t hartID, uint32_t irqID) {
  uint32_t enables = plic_mmio_read32(plic_enable_addr(hartID));
  enables &= ~(1u << irqID);
  plic_mmio_write32(plic_enable_addr(hartID), enables);
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

  /* TODO:去使能对应的CU IRQ */
  //   vPlicDeinit(0u, PLIC_IRQ_CU0);
  //   vPlicDeinit(1u, PLIC_IRQ_CU0);
  //   vPlicDeinit(2u, PLIC_IRQ_CU0);
  //   vPlicDeinit(3u, PLIC_IRQ_CU0);

  /*
   * ========== 多 CU 中断处理说明 ==========
   *
   * 1. PLIC IRQ 与 CU 的映射关系（定义在 plic_handler.h）：
   *    - PLIC_IRQ_CU0 = 11  ->  CU0 完成中断
   *    - PLIC_IRQ_CU1 = 12  ->  CU1 完成中断
   *    - PLIC_IRQ_CU2 = 13  ->  CU2 完成中断
   *    - PLIC_IRQ_CU3 = 14  ->  CU3 完成中断
   *
   * 2. 中断初始化（在 dag_runtime.c 的 vDagWorkerTask 中）：
   *    每个 worker 启动时为当前 hart 使能所有 CU 的 PLIC 中断：
   *      vPlicInit(portGET_CORE_ID(), PLIC_IRQ_CU0);
   *      vPlicInit(portGET_CORE_ID(), PLIC_IRQ_CU1);
   *      ...
   *
   * 3. 中断处理流程：
   *    a) 读取 PLIC claim 寄存器获取中断源号
   *    b) 完成 claim（写回 claim 寄存器）
   *    c) 根据 claim 计算 cu_id = claim - PLIC_IRQ_CU0
   *    d) 调用 vCuHandleIsr(cu_id, ...) 处理对应 CU 的完成事件
   *
   * 4. 并发支持：
   *    - 多个 CU 可同时运行（如 CU0 执行 nodeB，CU1 执行 nodeC）
   *    - 各自完成时触发各自的 PLIC 中断（IRQ 11, 12, ...）
   *    - ISR 会正确路由到对应的 gCuSlots[cu_id] 并通知等待者
   */
  if (claim >= PLIC_IRQ_CU0 && claim <= PLIC_IRQ_CU3) {
    /* 将 PLIC claim 映射为 cu_id（假定 CU IRQ 号连续分配） */
    uint32_t cu_id = claim - PLIC_IRQ_CU0;
    /* 调用 CU/DAG 桥接层处理该 CU 的完成事件 */
    vCuHandleIsr(cu_id, &xHigherPriorityTaskWoken);
  }
  return xHigherPriorityTaskWoken;
}

/* 在需要接收 PLIC 中断的 hart 上调用 */
void vEnableMachineExternalInterrupts(void) {
  const uint32_t mie_meie = (1u << 11);

  __asm__ volatile("csrs mie, %0" ::"r"(mie_meie));
}