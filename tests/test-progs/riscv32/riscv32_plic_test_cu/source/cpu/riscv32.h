#ifndef RISCV32_H_
#define RISCV32_H_
#include "types.h"

extern volatile int g_my_compute_done;

u32 handle_trap(u32 mcause, u32 mepc);
void handle_external_trap(void);

#endif /* RISCV32_H_ */
