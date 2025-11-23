cmake_minimum_required(VERSION 3.16)

project(FreeRTOS_QEMU_RV32_VIRT_Demo C ASM)

# Select boot mode: none (M-mode, PC=0x80000000) or opensbi (S-mode payload at 0x80200000)
# Usage: -DBOOT_MODE=none (default) or -DBOOT_MODE=opensbi
set(BOOT_MODE "none" CACHE STRING "Boot mode: none or opensbi")
set_property(CACHE BOOT_MODE PROPERTY STRINGS none opensbi)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# 选择合适的 objcopy：优先使用与交叉编译器同目录下的 RISC-V 版本
if(NOT CMAKE_OBJCOPY)
    get_filename_component(_CC_DIR "${CMAKE_C_COMPILER}" DIRECTORY)
    foreach(_ob riscv32-unknown-elf-objcopy riscv64-unknown-elf-objcopy)
        if(EXISTS "${_CC_DIR}/${_ob}")
            set(CMAKE_OBJCOPY "${_CC_DIR}/${_ob}" CACHE FILEPATH "objcopy tool" FORCE)
            break()
        endif()
    endforeach()
    if(NOT CMAKE_OBJCOPY)
        find_program(CMAKE_OBJCOPY NAMES riscv64-unknown-elf-objcopy riscv32-unknown-elf-objcopy objcopy)
        if(CMAKE_OBJCOPY)
            set(CMAKE_OBJCOPY "${CMAKE_OBJCOPY}" CACHE FILEPATH "objcopy tool" FORCE)
        endif()
    endif()
endif()

# 选项开关：软 tick 与 UART 调试字符
option(DemoSoftTick "在 -bios none 等情况下用 Idle 轮询驱动节拍" OFF)
option(DebugUart "输出极简 UART 调试字符 (A/T/S/E/F/W 等)" OFF)

# Point to FreeRTOS kernel 'Source' directory by default when not provided.
if(NOT FREERTOS_KERNEL_PATH)
    get_filename_component(FREERTOS_KERNEL_PATH "${CMAKE_CURRENT_LIST_DIR}/../../../Source" REALPATH)
endif()

set(FREERTOS_CONFIG_FILE_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}" CACHE PATH "Directory containing FreeRTOSConfig.h" FORCE)

add_subdirectory(.. FreeRTOS-Kernel-Port)

add_executable(freertos_virt_demo
    start.S
    main.c
    freertos_dag_example.c
    utils.c
)

set(TARGET_COMMON_FLAGS
    -ffreestanding
    -fno-builtin
    -fno-common
    -fno-exceptions
    -fno-unwind-tables
    -fno-asynchronous-unwind-tables
    -fno-stack-protector
    -O2
    -g
    -Wall
    -Wextra
    -Wno-unused-parameter
    -march=rv32imac_zicsr
    -mabi=ilp32
    -mcmodel=medany
)

target_compile_options(freertos_virt_demo PRIVATE ${TARGET_COMMON_FLAGS})

target_compile_options(freertos_virt_demo PRIVATE $<$<COMPILE_LANGUAGE:ASM>:-march=rv32imac_zicsr -mabi=ilp32>)

target_include_directories(freertos_virt_demo PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Choose linker script based on BOOT_MODE
set(LINKER_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/link.ld)
if(BOOT_MODE STREQUAL "opensbi")
    set(LINKER_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/link_sbi.ld)
elseif(BOOT_MODE STREQUAL "none")
    set(LINKER_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/link_none.ld)
endif()

message(STATUS "Linker script: ${LINKER_SCRIPT}")


target_link_options(freertos_virt_demo PRIVATE
    -nostartfiles
    -nostdlib
    -Wl,--gc-sections
    -Wl,-T,${LINKER_SCRIPT}
    -march=rv32imac_zicsr
    -mabi=ilp32
)

target_link_libraries(freertos_virt_demo
    FreeRTOS-Kernel
    FreeRTOS-Kernel-Heap4
    "-lgcc"
)

if(CMAKE_OBJCOPY)
    add_custom_command(TARGET freertos_virt_demo POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:freertos_virt_demo> $<TARGET_FILE_DIR:freertos_virt_demo>/freertos_virt_demo.bin
        COMMENT "Generating raw binary image")
endif()

if(DemoSoftTick)
    target_compile_definitions(freertos_virt_demo PRIVATE DEMO_SOFT_TICK=1)
    target_compile_definitions(FreeRTOS-Kernel INTERFACE DEMO_SOFT_TICK=1)
endif()

if(DebugUart)
    target_compile_definitions(freertos_virt_demo PRIVATE DEBUG_UART=1)
    target_compile_definitions(FreeRTOS-Kernel INTERFACE DEBUG_UART=1)
endif()
