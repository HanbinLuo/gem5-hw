/*
 * FreeRTOS Kernel <DEVELOPMENT BRANCH>
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef PORTCONTEXT_H
#define PORTCONTEXT_H

#include "freertos_risc_v_chip_specific_extensions.h"

#if __riscv_xlen == 64
    #define portWORD_SIZE    8
    #define store_x          sd
    #define load_x           ld
    #define portPOINTER_SHIFT    3
#elif __riscv_xlen == 32
    #define store_x          sw
    #define load_x           lw
    #define portWORD_SIZE    4
    #define portPOINTER_SHIFT    2
#else
    #error Assembler did not define __riscv_xlen
#endif

#ifdef __riscv_32e
    #define portCONTEXT_SIZE               ( 15 * portWORD_SIZE )
    #define portCRITICAL_NESTING_OFFSET    13
    #define portMSTATUS_OFFSET             14
#else
    #define portCONTEXT_SIZE               ( 31 * portWORD_SIZE )
    #define portCRITICAL_NESTING_OFFSET    29
    #define portMSTATUS_OFFSET             30
#endif

.extern pxCurrentTCBArray
.extern xCriticalNestingArray
.extern xISRStackTops

.macro portcontextGET_CORE_ID out_reg
    csrr \out_reg, mhartid
.endm

.macro portcontextGET_CURRENT_TCB_FOR_CORE core_reg, out_reg
    la \out_reg, pxCurrentTCBArray
    load_x \out_reg, 0 ( \out_reg )
    slli t2, \core_reg, portPOINTER_SHIFT
    add \out_reg, \out_reg, t2
    load_x \out_reg, 0 ( \out_reg )
.endm

.macro portcontextGET_CRITICAL_NESTING_PTR_FOR_CORE core_reg, out_reg
    load_x \out_reg, xCriticalNestingArray
    slli t2, \core_reg, portPOINTER_SHIFT
    add \out_reg, \out_reg, t2
.endm

.macro portcontextSWITCH_TO_ISR_STACK reg_pISRStackTop, reg_ISRStackTop
    csrr \reg_pISRStackTop, mhartid
    la \reg_ISRStackTop, xISRStackTops
    slli t2, \reg_pISRStackTop, portPOINTER_SHIFT
    add \reg_ISRStackTop, \reg_ISRStackTop, t2
    load_x \reg_ISRStackTop, 0 ( \reg_ISRStackTop )
    beqz \reg_ISRStackTop, no_isr_stack\@
    mv sp, \reg_ISRStackTop
no_isr_stack\@:
.endm

/*
 * Full context save/restore macros for RV32/64, SMP-aware.
 * These mirror the upstream FreeRTOS RISC-V port, adapted to use
 * pxCurrentTCBArray[] and xCriticalNestingArray[] per-core storage.
 */

    .macro portcontextSAVE_CONTEXT_INTERNAL
addi sp, sp, -portCONTEXT_SIZE
store_x x1, 1 * portWORD_SIZE( sp )
store_x x5, 2 * portWORD_SIZE( sp )
store_x x6, 3 * portWORD_SIZE( sp )
store_x x7, 4 * portWORD_SIZE( sp )
store_x x8, 5 * portWORD_SIZE( sp )
store_x x9, 6 * portWORD_SIZE( sp )
store_x x10, 7 * portWORD_SIZE( sp )
store_x x11, 8 * portWORD_SIZE( sp )
store_x x12, 9 * portWORD_SIZE( sp )
store_x x13, 10 * portWORD_SIZE( sp )
store_x x14, 11 * portWORD_SIZE( sp )
store_x x15, 12 * portWORD_SIZE( sp )
#ifndef __riscv_32e
    store_x x16, 13 * portWORD_SIZE( sp )
    store_x x17, 14 * portWORD_SIZE( sp )
    store_x x18, 15 * portWORD_SIZE( sp )
    store_x x19, 16 * portWORD_SIZE( sp )
    store_x x20, 17 * portWORD_SIZE( sp )
    store_x x21, 18 * portWORD_SIZE( sp )
    store_x x22, 19 * portWORD_SIZE( sp )
    store_x x23, 20 * portWORD_SIZE( sp )
    store_x x24, 21 * portWORD_SIZE( sp )
    store_x x25, 22 * portWORD_SIZE( sp )
    store_x x26, 23 * portWORD_SIZE( sp )
    store_x x27, 24 * portWORD_SIZE( sp )
    store_x x28, 25 * portWORD_SIZE( sp )
    store_x x29, 26 * portWORD_SIZE( sp )
    store_x x30, 27 * portWORD_SIZE( sp )
    store_x x31, 28 * portWORD_SIZE( sp )
#endif

/* Save mstatus and critical nesting for this core. */
csrr t0, mstatus
store_x t0, portMSTATUS_OFFSET * portWORD_SIZE( sp )

portcontextGET_CORE_ID t1
portcontextGET_CRITICAL_NESTING_PTR_FOR_CORE t1, t0
load_x t2, 0( t0 )
store_x t2, portCRITICAL_NESTING_OFFSET * portWORD_SIZE( sp )

/* Chip-specific additional registers. */
portasmSAVE_ADDITIONAL_REGISTERS

/* Save SP into current TCB for this core. */
portcontextGET_CURRENT_TCB_FOR_CORE t1, t0
store_x sp, 0( t0 )
    .endm

    .macro portcontextSAVE_EXCEPTION_CONTEXT
portcontextSAVE_CONTEXT_INTERNAL
csrr a0, mcause
csrr a1, mepc
addi a1, a1, 4
store_x a1, 0 ( sp )
/* Switch to per-core ISR stack if configured. */
portcontextSWITCH_TO_ISR_STACK t0, t1
    .endm

    .macro portcontextSAVE_INTERRUPT_CONTEXT
portcontextSAVE_CONTEXT_INTERNAL
csrr a0, mcause
csrr a1, mepc
store_x a1, 0 ( sp )
/* Switch to per-core ISR stack if configured. */
portcontextSWITCH_TO_ISR_STACK t0, t1
    .endm

    .macro portcontextRESTORE_CONTEXT
/* Load SP from current TCB for this core. */
portcontextGET_CORE_ID t1
portcontextGET_CURRENT_TCB_FOR_CORE t1, t0
load_x sp, 0 ( t0 )

/* mepc <- task return address */
load_x t0, 0 ( sp )
csrw mepc, t0

/* Chip-specific restore. */
portasmRESTORE_ADDITIONAL_REGISTERS

/* Restore mstatus and critical nesting. */
load_x t0, portMSTATUS_OFFSET * portWORD_SIZE( sp )
csrw mstatus, t0

portcontextGET_CRITICAL_NESTING_PTR_FOR_CORE t1, t0
load_x t2, portCRITICAL_NESTING_OFFSET * portWORD_SIZE( sp )
store_x t2, 0 ( t0 )

/* General registers. */
load_x x1, 1 * portWORD_SIZE( sp )
load_x x5, 2 * portWORD_SIZE( sp )
load_x x6, 3 * portWORD_SIZE( sp )
load_x x7, 4 * portWORD_SIZE( sp )
load_x x8, 5 * portWORD_SIZE( sp )
load_x x9, 6 * portWORD_SIZE( sp )
load_x x10, 7 * portWORD_SIZE( sp )
load_x x11, 8 * portWORD_SIZE( sp )
load_x x12, 9 * portWORD_SIZE( sp )
load_x x13, 10 * portWORD_SIZE( sp )
load_x x14, 11 * portWORD_SIZE( sp )
load_x x15, 12 * portWORD_SIZE( sp )
#ifndef __riscv_32e
    load_x x16, 13 * portWORD_SIZE( sp )
    load_x x17, 14 * portWORD_SIZE( sp )
    load_x x18, 15 * portWORD_SIZE( sp )
    load_x x19, 16 * portWORD_SIZE( sp )
    load_x x20, 17 * portWORD_SIZE( sp )
    load_x x21, 18 * portWORD_SIZE( sp )
    load_x x22, 19 * portWORD_SIZE( sp )
    load_x x23, 20 * portWORD_SIZE( sp )
    load_x x24, 21 * portWORD_SIZE( sp )
    load_x x25, 22 * portWORD_SIZE( sp )
    load_x x26, 23 * portWORD_SIZE( sp )
    load_x x27, 24 * portWORD_SIZE( sp )
    load_x x28, 25 * portWORD_SIZE( sp )
    load_x x29, 26 * portWORD_SIZE( sp )
    load_x x30, 27 * portWORD_SIZE( sp )
    load_x x31, 28 * portWORD_SIZE( sp )
#endif
addi sp, sp, portCONTEXT_SIZE
mret
    .endm

#endif /* PORTCONTEXT_H */
