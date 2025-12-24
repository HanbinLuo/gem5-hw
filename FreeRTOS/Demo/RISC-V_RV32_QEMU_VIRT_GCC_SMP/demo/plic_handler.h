#ifndef PLIC_HANDLER_H
#define PLIC_HANDLER_H

#include <stdint.h>

#include "FreeRTOS.h"

/*
 * gem5 / QEMU virt PLIC 配置：
 * 假定：
 *   - PLIC 基址为 0x0C00_0000
 *   - CU0 的 DMA 完成中断源号为 11
 *   - 只使用 context 0（例如 machine external interrupt）
 */

#define PLIC_BASE 0x0C000000u
#define PLIC_CONTEXT 0u
#define PLIC_IRQ_CU0      11u
#define PLIC_IRQ_CU_LAST  (PLIC_IRQ_CU0 + CU_MAX_COUNT - 1)

void vPlicInit(uint32_t hartID, uint32_t irqID);
void vPlicDeinit(uint32_t hartID, uint32_t irqID);
void vEnableMachineExternalInterrupts(void);

#endif /* PLIC_HANDLER_H */
