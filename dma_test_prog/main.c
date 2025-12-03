/* main.c */
#include <stdint.h>

#define DMA_BASE 0x50000000u

typedef struct {
    volatile uint32_t SRC;     // 0x00
    volatile uint32_t DST;     // 0x04
    volatile uint32_t LEN;     // 0x08
    volatile uint32_t CTRL;    // 0x0C
    volatile uint32_t STATUS;  // 0x10
} simple_dma_regs_t;

#define DMA_CTRL_START   (1u << 0)
#define DMA_STATUS_DONE  (1u << 0)

static simple_dma_regs_t * const dma =
    (simple_dma_regs_t *)DMA_BASE;

/* 假设 DRAM 起始 0x80000000，这里随便两个 buffer */
#define SRC_BUF  0x80001000u
#define DST_BUF  0x80002000u

static void dma_memcpy(void *dst, const void *src, uint32_t len)
{
    /* 清 DONE 标志 */
    dma->STATUS = DMA_STATUS_DONE;

    dma->SRC  = (uint32_t)src;
    dma->DST  = (uint32_t)dst;
    dma->LEN  = len;
    dma->CTRL = DMA_CTRL_START;

    /* 简单轮询等待 DMA 完成 */
    while ((dma->STATUS & DMA_STATUS_DONE) == 0) {
        /* busy wait */
    }
}

/* 一个极简裸机 main：用 DMA 拷贝一块内存，然后自检 */
int main(void)
{
    uint8_t *src = (uint8_t *)SRC_BUF;
    uint8_t *dst = (uint8_t *)DST_BUF;
    const uint32_t len = 256;

    /* 初始化 src，dst 先清零 */
    for (uint32_t i = 0; i < len; ++i) {
        src[i] = (uint8_t)(i & 0xFF);
        dst[i] = 0;
    }

    /* 调用 DMA 搬运 */
    dma_memcpy(dst, src, len);

    /* 校验结果 */
    int ok = 1;
    for (uint32_t i = 0; i < len; ++i) {
        if (dst[i] != src[i]) {
            ok = 0;
            break;
        }
    }

    /* 裸机环境没有 printf，就用“死循环”表示状态：
     * ok = 1 -> 死循环 A；ok = 0 -> 死循环 B
     * 你可以在仿真里看寄存器、内存或者加上 UART 输出。
     */
    if (ok) {
        while (1) {
            /* 成功状态，可以在这里插 NOP 等待调试 */
            asm volatile ("nop");
        }
    } else {
        while (1) {
            asm volatile ("nop");
        }
    }

    return 0;
}