if (DEFINED ENV{FREERTOS_KERNEL_PATH} AND (NOT FREERTOS_KERNEL_PATH))
    set(FREERTOS_KERNEL_PATH $ENV{FREERTOS_KERNEL_PATH})
    message(STATUS "Using FREERTOS_KERNEL_PATH from environment ('${FREERTOS_KERNEL_PATH}')")
endif ()

# Derive a sensible default: point to FreeRTOS/Source relative to this file.
if (NOT FREERTOS_KERNEL_PATH)
    get_filename_component(_CURRENT_DIR ${CMAKE_CURRENT_LIST_DIR} REALPATH)
    # This repository layout: FreeRTOS/.../RISC-V_RV32_QEMU_VIRT_GCC_SMP
    # The kernel core sources live at FreeRTOS/Source.
    set(_DEFAULT_KERNEL_PATH "${_CURRENT_DIR}/../../Source")
    if (EXISTS "${_DEFAULT_KERNEL_PATH}/tasks.c")
        set(FREERTOS_KERNEL_PATH "${_DEFAULT_KERNEL_PATH}")
    endif ()
endif ()

if (NOT FREERTOS_KERNEL_PATH)
    message(FATAL_ERROR "FreeRTOS location was not specified. Please set FREERTOS_KERNEL_PATH (expected FreeRTOS/Source).")
endif ()

set(FREERTOS_KERNEL_PATH "${FREERTOS_KERNEL_PATH}" CACHE PATH "Path to the FreeRTOS Kernel 'Source' directory")
get_filename_component(FREERTOS_KERNEL_PATH "${FREERTOS_KERNEL_PATH}" REALPATH BASE_DIR "${CMAKE_BINARY_DIR}")
if (NOT EXISTS ${FREERTOS_KERNEL_PATH})
    message(FATAL_ERROR "Directory '${FREERTOS_KERNEL_PATH}' not found")
endif ()

# We provide our own port (port.c/portASM.S) in this repo, so do not try to
# add a port directory from the kernel tree.
