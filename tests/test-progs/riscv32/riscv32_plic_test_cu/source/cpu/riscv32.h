#ifndef RISCV32_H_
#define RISCV32_H_
#include "types.h"

// 状态定义
typedef enum
{
    STATE_IDLE,
    STATE_DMA_IN_PROGRESS,
    STATE_COMPUTING,
    STATE_DMA_OUT_PROGRESS,
    STATE_DONE
} work_state_t;

extern volatile work_state_t g_work_state;

u32 handle_trap(u32 mcause, u32 mepc);
void handle_external_trap(void);

#endif /* RISCV32_H_ */
