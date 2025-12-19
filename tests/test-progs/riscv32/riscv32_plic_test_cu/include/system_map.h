#ifndef SYSTEM_MAP_H
#define SYSTEM_MAP_H

#include <stdint.h>

#define CU_BASE    0x10009000UL
/* New ComputeUnit register map (all offsets are byte addresses)
        0x00..0x03 : CU_ID    (32-bit)
        0x04..0x07 : JOB_ID   (32-bit)
        0x08..0x0B : SIZE     (32-bit)
        0x0C..0x0F : DELAY    (32-bit, ticks)
        0x10        : CONFIG   (write to start, 8-bit)
        0x11        : STATUS   (read, bit0 = done)
        0x12        : BUSY     (read, bit0 = busy)
*/
#define CU_CU_ID   (CU_BASE + 0x00)
#define CU_JOB_ID  (CU_BASE + 0x04)
#define CU_SIZE    (CU_BASE + 0x08)
#define CU_DELAY   (CU_BASE + 0x0C)
#define CU_CONFIG  (CU_BASE + 0x10)
#define CU_STATUS  (CU_BASE + 0x11)
#define CU_BUSY    (CU_BASE + 0x12)

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

static inline uint8_t mmio_read8(uintptr_t addr) {
        volatile uint8_t *p = (volatile uint8_t *)addr;
        return *p;
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
