/*
 * Minimal machine-mode start-up for the FreeRTOS QEMU RV32 virt demo.
 */

    .section .init, "ax", @progbits
    .globl _start

.equ STACK_SIZE_PER_HART, 0x8000

_start:
    /* Set up global pointer for small data references. */
    .option push
    .option norelax
    la gp, __global_pointer$
    .option pop

    /* Allocate a private stack for each hart from the top of RAM. */
    la  sp, __stack_top
    csrr t0, mhartid
    addi t1, t0, 1
    li  t2, STACK_SIZE_PER_HART
    mul t1, t1, t2
    sub sp, sp, t1
    addi sp, sp, -16
    andi sp, sp, -16

    /* 早期调试：向 UART 打印一个 '>'，确认已进入 _start。 */
    li  t3, 0x10000000
    lb  t4, 5(t3)
    andi t4, t4, 0x20
    beqz t4, 1f
    li  t4, '>'
    sb  t4, 0(t3)
1:

#ifdef EARLY_MTVEC
    /* 可选：提前设置 mtvec 为向量模式，指向 FreeRTOS 的向量表。 */
    .option push
    .option norelax
    la  t0, freertos_vector_table
    .option pop
    ori t1, t0, 1              /* Vectored 模式 (mtvec.MODE=1) */
    csrw mtvec, t1
#endif

    /* Hart 0 clears .bss then signals the others to continue. */
    bnez t0, 2f

    la  t1, __bss_start__
    la  t2, __bss_end__
1:
    bgeu t1, t2, 3f
    sw   zero, 0(t1)
    addi t1, t1, 4
    j    1b
3:
    li  t1, 1
    la  t2, __bss_init_flag
    sw  t1, 0(t2)
    fence w, rw
    j   4f

2:
    la  t1, __bss_init_flag
5:
    lw  t2, 0(t1)
    beqz t2, 5b
    fence r, rw

4:
    tail main

    /* ========= 单独定义 __bss_init_flag 为 BSS/SBSS 变量 ========= */

    .section .sbss, "aw", @nobits
    .globl __bss_init_flag
    .p2align 2           /* 2^2 = 4 字节对齐 */
__bss_init_flag:
    .zero 4              /* BSS 段用 .zero，而不是 .word  */
