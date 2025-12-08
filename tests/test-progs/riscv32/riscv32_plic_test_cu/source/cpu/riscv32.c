#include "riscv.h"
#include "riscv32.h"
#include "ns16550.h"
#include "printf/printf.h"
#include "clint.h"
#include "plic.h"
#include "system_map.h"

#define MY_COMPUTE_IRQ 11
#define DMA_IRQ 12

volatile work_state_t g_work_state = STATE_IDLE;

u32 g_sys_tick = 0;

u32 handle_trap(u32 mcause, u32 mepc)
{
	int is_interrupt = mcause & 0x80000000;

	//printf("mcause: 0x%08x\n", mcause);
	//printf("mepc: 0x%08x\n", mepc);
	//printf("is_interrupt: 0x%08x\n", is_interrupt);

	mcause = mcause & 0xFF;
	if (is_interrupt) {
		switch (mcause) {
        case IRQ_M_SOFT: // soft
			clint_set_msip(0, 0); // clear soft isr
            printf("soft isr: mcause %d\n", mcause);
			break;

		case IRQ_M_TIMER: // mtime
			clint_add_mtimecmp(D_CLOCK_RATE, 0);
            ++g_sys_tick;
			// test only
			if ((g_sys_tick & 3) == 0)
				clint_set_msip(1, 0);
			// debug only
            printf("mtime: %d\n", g_sys_tick);
			break;

		case IRQ_M_EXT: // external
			handle_external_trap();
			break;

		default:
			printf("unknow isr: %d\n", mcause);
			break;
		}
	} else {
        printf("exception:\n");
        printf("mcause: 0x%08x\n", mcause);
        printf("mepc: 0x%08x\n", mepc);
    }

	//printf("mret");
	return is_interrupt ? mepc : (mepc + 4);
}

void handle_external_trap(void)
{
	int irq = plic_claim();
	if (irq == UART0_IRQ) {
		UartIsr();
        } else if (irq == DMA_IRQ) {
        // DMA 中断处理
        if (g_work_state == STATE_DMA_IN_PROGRESS) {
            printf("[ISR] DMA In Done -> Start Compute\n");
            // 启动计算
            mmio_write8(CU_LENGTH, 16);
            mmio_write8(CU_CONFIG, 0); // 0 = add
            g_work_state = STATE_COMPUTING;
        } else if (g_work_state == STATE_DMA_OUT_PROGRESS) {
            printf("[ISR] DMA Out Done -> All Finished\n");
            g_work_state = STATE_DONE;
        } else {
            printf("[ISR] Unexpected DMA IRQ in state %d\n", g_work_state);
        }
        // 清除 DMA 中断状态（如果硬件需要，这里 SimpleDMA 只要写 0x10 即可清除 done）
        // 但 SimpleDMA 的 done 是只读的吗？看代码 write 0x10 可以清除 done
        simple_dma_regs_t * const dma = (simple_dma_regs_t *)DMA_BASE;
        dma->STATUS = 1; // Clear done bit
    } else if (irq == MY_COMPUTE_IRQ) {
        // 计算单元中断处理
                printf("[ISR] Compute Done -> Start DMA Out\n");
        if (g_work_state == STATE_COMPUTING) {
            // 启动 DMA 回写: CU_RESULT -> DST_BUF
            dma_start_async(CU_RESULT, DST_BUF, 16);
            g_work_state = STATE_DMA_OUT_PROGRESS;
        } else {
             printf("[ISR] Unexpected Compute IRQ "
            "in state %d\n", g_work_state);
        }
        } else {
                printf("unknow external isr: %d\n", irq);
        }

	if(irq)
      plic_complete(irq);
}
