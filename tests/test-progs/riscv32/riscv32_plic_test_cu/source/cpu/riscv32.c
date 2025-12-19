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
    if (irq) {
        // 简化处理中断：只打印发生了哪个中断，然后完成它
        // printf("[ISR] External IRQ %d received\n", irq);
        plic_complete(irq);
    } else {
        // printf("[ISR] External IRQ claim returned 0\n");
    }
}
