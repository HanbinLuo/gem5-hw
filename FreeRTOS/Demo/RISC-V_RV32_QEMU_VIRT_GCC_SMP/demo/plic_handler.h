#ifndef PLIC_HANDLER_H
#define PLIC_HANDLER_H

#include <stdint.h>

#include "FreeRTOS.h"
#include "cu_offload.h"
#include "task.h"

/*
 * gem5 / QEMU virt PLIC 配置：
 * 假定：
 *   - PLIC 基址为 0x0C00_0000
 *   - CU0 的 DMA 完成中断源号为 11
 *   - 只使用 context 0（例如 machine external interrupt）
 */

#define PLIC_BASE 0x0C000000u
#define PLIC_CONTEXT 0u
#define PLIC_IRQ_CU0 11u

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
static inline void vPlicInit(void) {
  plic_mmio_write32(plic_priority_addr(PLIC_IRQ_CU0), 1u);

  uint32_t enables = plic_mmio_read32(plic_enable_addr(PLIC_CONTEXT));
  enables |= (1u << PLIC_IRQ_CU0);
  plic_mmio_write32(plic_enable_addr(PLIC_CONTEXT), enables);

  plic_mmio_write32(plic_threshold_addr(PLIC_CONTEXT), 0u);
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

  uint32_t claim = plic_mmio_read32(plic_claim_addr(PLIC_CONTEXT));
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  if (claim == PLIC_IRQ_CU0) {
    /* 把 “CU0 完成” 这个事件交给 CU/DAG 桥接层 */
    vCuHandleIsr(0u, &xHigherPriorityTaskWoken);
  }

  if (claim != 0u) {
    plic_mmio_write32(plic_claim_addr(PLIC_CONTEXT), claim);
  }

  return xHigherPriorityTaskWoken;
}

/* 在需要接收 PLIC 中断的 hart 上调用 */
static inline void vEnableMachineExternalInterrupts(void) {
  const uint32_t mie_meie = (1u << 11);

  __asm__ volatile("csrs mie, %0" ::"r"(mie_meie));
}

#endif /* PLIC_HANDLER_H */
