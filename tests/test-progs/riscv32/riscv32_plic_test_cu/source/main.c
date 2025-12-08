#include "riscv.h"
#include "riscv32.h"
#include "riscv-virt.h"
#include "printf/printf.h"
#include "system_map.h"

// 读 RISC-V mcycle（裸机运行在 machine/privileged 模式），用于测周期数
static inline unsigned long long rdcycle64(void) {
unsigned long long v;
__asm__ volatile("rdcycle %0" : "=r"(v));
return v;
}

int main(void)
{
plt_virt_init();

uint8_t *src = (uint8_t *)SRC_BUF;
uint8_t *dst = (uint8_t *)DST_BUF;
const uint32_t len = 256;

/* 初始化 src，dst 先清零 */
for (uint32_t i = 0; i < len; ++i) {
src[i] = (uint8_t)(i & 0xFF);
dst[i] = 0;
}

    printf("Starting Interrupt-Driven Compute Test...\n");

unsigned long long t0 = rdcycle64();

    // 1. 启动流程：DMA 搬入 (SRC -> CU_OP_A)
    // 后续流程由 ISR 接管：
    // DMA In Done (IRQ 12) -> Start Compute
    // Compute Done (IRQ 11) -> Start DMA Out
    // DMA Out Done (IRQ 12) -> Set STATE_DONE

    g_work_state = STATE_DMA_IN_PROGRESS;

    // MyCompute 的寄存器布局：OP_A(0x00, 16B), OP_B(0x10, 16B). 连续的 32 字节。
    // 所以我们可以一次搬运 32 字节到 CU_OP_A。
    dma_start_async(SRC_BUF, CU_OP_A, 32);

// 等待完成
while (g_work_state != STATE_DONE) {
// 等待中断
        // asm volatile("wfi"); // 可选，省电
}
printf("检测到全流程完成 (STATE_DONE)\n");

unsigned long long t1 = rdcycle64();

unsigned long long cycles = t1 - t0;
for (int i = 0; i < 16; i++) {
printf("[%d] %u + %u = %u\n", i, src[i], src[i + 16], dst[i]);
}
printf("Total cycles: %d \n", (int)cycles);

return 0;
}
