# FreeRTOS SMP port for QEMU RV32 virt

This directory contains a community-supported FreeRTOS SMP port that targets the
`virt` machine provided by QEMU when running 32-bit RISC-V harts. It reuses the
standard CLINT timer (mtime/mtimecmp) for system ticks and the MSIP registers for
cross-core inter-processor interrupts.

## Using this port

1. Add this directory to your project either by calling
   `add_subdirectory(<path-to-this-dir> FreeRTOS-Kernel)` or by including
   `FreeRTOS_Kernel_import.cmake` before your top-level `project()` invocation.
2. Provide the usual FreeRTOS kernel sources via `FREERTOS_KERNEL_PATH` (environment
   variable or CMake cache entry).
3. In your `FreeRTOSConfig.h`:

    - Set `configNUMBER_OF_CORES` to the number of QEMU harts you plan to use.
    - Set `configTICK_CORE` to the hart that should service the SysTick (defaults to 0).
    - Optionally override `configCLINT_BASE_ADDRESS`, `configCLINT_CLOCK_HZ`,
      `configCLINT_MSIP_STRIDE`, or `configCLINT_MTIMER_STRIDE` if your platform
      deviates from the QEMU defaults (0x0200_0000 base, 10 MHz CLINT clock).
    - Leave `configUSE_ISR_STACK` as 0 unless you provide per-core ISR stacks yourself.

4. During start-up call `xPortStartScheduler()` on the hart selected by
   `configTICK_CORE`. On every other hart call `vPortStartSchedulerOnSecondaryCore()`
   after basic platform initialisation (stack, global pointer, etc.). All calls
   must be made with interrupts disabled.

### External interrupts

The port handles machine timer and software interrupts directly. For additional
interrupt controllers (e.g. PLIC) provide an implementation of
`BaseType_t xPortHandleExternalInterrupt(uint32_t mcause, uint32_t mepc)` in your
application. Return `pdTRUE` when a context switch should be requested.

### Inter-core notifications

`portYIELD_CORE(coreId)` raises an MSIP interrupt on the specified core which in
turn forces a yield on that core.

## Validation tips

-   Run the FreeRTOS SMP demos inside QEMU, for example:
    ```
    qemu-system-riscv32 -machine virt -m 32M -smp 2 -nographic \
        -kernel <your-app.elf> -bios default -serial mon:stdio
    ```
-   Confirm that machine timer interrupts are delivered on the tick core and that
    MSIP interrupts trigger rescheduling on other cores when using SMP-aware tests.
-   Consider enabling `configASSERT` and tracing hooks while validating the port on
    new hardware.

## Demo Program

A minimal SMP example is provided under [`demo`](demo). It launches one task pinned to
hart 0 that emits a heartbeat once per second and pings a worker task pinned to hart 1.
The worker performs a small checksum workload each time it is notified, demonstrating
cross-core task notifications and MSIP-driven inter-processor interrupts.

### Building the demo

```
mkdir -p demo/build
cmake -S demo -B demo/build -G Ninja \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -DFREERTOS_KERNEL_PATH=/home/fengyuan/FreeRTOSv202411.00/FreeRTOS/Source \
    -DCMAKE_C_COMPILER=riscv64-unknown-elf-gcc \
    -DCMAKE_ASM_COMPILER=riscv64-unknown-elf-gcc \
    -DCMAKE_OBJCOPY=riscv64-unknown-elf-objcopy
cmake --build demo/build
```

The resulting ELF image is `demo/build/freertos_virt_demo` and a raw binary is emitted as
`demo/build/freertos_virt_demo.bin` when `objcopy` is available.

### Running under QEMU

Launch the demo on two harts in machine mode (no firmware) with:

```
qemu-system-riscv32 -machine virt -m 32M -smp 2 -nographic -bios none \
    -kernel demo/build/freertos_virt_demo
```

Typical output:

```
FreeRTOS SMP demo (QEMU RV32 virt)
[core 0] heartbeat 0
[core 1] worker 0 checksum=0x933E01F0
[core 0] heartbeat 1
[core 1] worker 1 checksum=0x27D313E8
...
```

Stop QEMU with `Ctrl+A` followed by `x` (when using the default `-nographic` console).
