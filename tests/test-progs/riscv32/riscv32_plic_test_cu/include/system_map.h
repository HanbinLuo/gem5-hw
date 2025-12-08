#ifndef SYSTEM_MAP_H
#define SYSTEM_MAP_H

#include <stdint.h>

#define CU_BASE    0x10009000UL
#define CU_OP_A    (CU_BASE + 0x00)
#define CU_OP_B    (CU_BASE + 0x10)
#define CU_RESULT  (CU_BASE + 0x20)
#define CU_LENGTH  (CU_BASE + 0x30)
#define CU_CONFIG  (CU_BASE + 0x31)
#define CU_STATUS  (CU_BASE + 0x32)

#define DMA_BASE 0x50000000u

typedef struct
{
        volatile uint32_t SRC;     // 0x00
        volatile uint32_t DST;     // 0x04
        volatile uint32_t LEN;     // 0x08
        volatile uint32_t CTRL;    // 0x0C
        volatile uint32_t STATUS;  // 0x10
} simple_dma_regs_t;

#define DMA_CTRL_START   (1u << 0)
#define DMA_STATUS_DONE  (1u << 0)

#define SRC_BUF  0x90001000u
#define DST_BUF  0x90002000u

static inline void mmio_write8(uintptr_t addr, uint8_t v) {
        volatile uint8_t *p = (volatile uint8_t *)addr;
        *p = v;
}

static inline void dma_start_async(uint32_t src, uint32_t dst, uint32_t len) {
    simple_dma_regs_t * const dma = (simple_dma_regs_t *)DMA_BASE;
    dma->STATUS = DMA_STATUS_DONE; // Clear status
    dma->SRC = src;
    dma->DST = dst;
    dma->LEN = len;
    dma->CTRL = DMA_CTRL_START;
}

#endif
