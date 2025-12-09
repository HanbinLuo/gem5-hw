/* start.S */
    .section .init
    .globl _start

_start:
    /* 设置栈指针，假设内存从 0x80000000 开始，往上随便留 16KB 给栈 */
    la sp, _stack_top

    /* 清 BSS 段 */
    la  t0, __bss_start
    la  t1, __bss_end
1:
    bge t0, t1, 2f
    sw  x0, 0(t0)
    addi t0, t0, 4
    j   1b
2:

    /* 调用 C 的 main 函数 */
    call main

    /* main 返回就死循环 */
3:
    j 3b

    .section .bss
    .globl __bss_start
__bss_start:
    .space 4        /* 只是标记，真正大小由链接脚本控制 */

    .globl __bss_end
__bss_end:
    .space 4

    .section .stack
    .globl _stack_top
_stack_top:
    .space 0x4000   /* 16KB 栈空间，只是占个位置 */