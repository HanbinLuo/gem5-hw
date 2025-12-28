ENTRY(_start)

/* Create separate RX and RW PT_LOAD segments to avoid RWX. */
PHDRS
{
    /* Use numeric flags: PF_R=4, PF_W=2, PF_X=1 */
    text PT_LOAD FLAGS(0x5); /* R|X */
    data PT_LOAD FLAGS(0x6); /* R|W */
}

MEMORY
{
    /* Bare-metal (-bios none): QEMU sets PC to 0x80000000 */
    RAM (rwx) : ORIGIN = 0x80000000, LENGTH = 512M
}

STACK_SIZE_PER_HART = 0x8000;

SECTIONS
{
    .text :
    {
        KEEP(*(.init))
        *(.text*)
        *(.time_critical*)
        *(.rodata*)
    } > RAM :text

    /* Place FreeRTOS vectored trap table close to text to keep short jumps. */
    . = ALIGN(64);
    .freertos_vectors :
    {
        KEEP(*(.freertos.vectors))
    } > RAM :text

    . = ALIGN(16);
    __data_start__ = .;
    .data :
    {
        *(.data*)
        *(.sdata*)
    } > RAM :data
    __data_end__ = .;

    /* Set global pointer to the middle of the small data area to allow
       12-bit signed offsets to cover .sdata/.sbss. */
    PROVIDE(__global_pointer$ = __data_start__ + 0x800);

    . = ALIGN(16);
    __bss_start__ = .;
    .bss (NOLOAD) :
    {
        *(.sbss*)
        *(.bss*)
        *(COMMON)
    } > RAM :data
    __bss_end__ = .;

    . = ALIGN(16);
    __stack_top = ORIGIN(RAM) + LENGTH(RAM);
    PROVIDE(__StackTop = __stack_top);
    PROVIDE(__StackOneTop = __stack_top - STACK_SIZE_PER_HART);
    PROVIDE(__freertos_irq_stack_top = __stack_top);
}
