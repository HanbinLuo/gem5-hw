cmake_minimum_required(VERSION 3.13)

if (NOT TARGET _FreeRTOS_kernel_inclusion_marker)
    add_library(_FreeRTOS_kernel_inclusion_marker INTERFACE)

    if (NOT FREERTOS_KERNEL_PATH)
        get_filename_component(FREERTOS_KERNEL_PATH "${CMAKE_CURRENT_LIST_DIR}/../../../../.." REALPATH)
    endif ()

    message(DEBUG "FREERTOS_KERNEL_PATH is ${FREERTOS_KERNEL_PATH}")
    project(FreeRTOS-Kernel C)

    set(CMAKE_C_STANDARD 11)
endif ()

include(${CMAKE_CURRENT_LIST_DIR}/library.cmake)
