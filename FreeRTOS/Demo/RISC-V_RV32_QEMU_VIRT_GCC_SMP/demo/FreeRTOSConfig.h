#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <stdint.h>
#include "virt_config.h"

/*-----------------------------------------------------------*/
/* Core kernel settings. */
/*-----------------------------------------------------------*/

#define configUSE_PREEMPTION                    1
#define configUSE_TIME_SLICING                  1
#define configUSE_TICKLESS_IDLE                 0
#define configCPU_CLOCK_HZ                      ( configCLINT_CLOCK_HZ )
#define configTICK_RATE_HZ                      ( 1000UL )
#define configMAX_PRIORITIES                    7
#define configMINIMAL_STACK_SIZE                256
#define configMAX_TASK_NAME_LEN                 16
#define configUSE_16_BIT_TICKS                  0
#define configIDLE_SHOULD_YIELD                 1
#define configUSE_TASK_NOTIFICATIONS            1
#define configUSE_MUTEXES                       1
#define configUSE_RECURSIVE_MUTEXES             1
#define configUSE_COUNTING_SEMAPHORES           1
#define configUSE_QUEUE_SETS                    0
#define configSUPPORT_DYNAMIC_ALLOCATION        1
#define configSUPPORT_STATIC_ALLOCATION         0
#define configTOTAL_HEAP_SIZE                   ( ( size_t ) ( 64U * 1024U ) )
#define configUSE_APPLICATION_TASK_TAG          0
#define configUSE_CORE_AFFINITY                 1
#define configNUMBER_OF_CORES                   4
#define configTICK_CORE                         0
#define configRUN_MULTIPLE_PRIORITIES           1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0
#define configMESSAGE_BUFFER_LENGTH_TYPE        size_t
#define configUSE_NEWLIB_REENTRANT              0
#define configCHECK_FOR_STACK_OVERFLOW          2
#define configUSE_MALLOC_FAILED_HOOK            1
#define configUSE_IDLE_HOOK                     1
#define configUSE_TIMERS                        1
/* Lower timer task priority below demo tasks for bring-up. */
#define configTIMER_TASK_PRIORITY               ( tskIDLE_PRIORITY + 0 )
#define configTIMER_QUEUE_LENGTH                16
#define configTIMER_TASK_STACK_DEPTH            ( configMINIMAL_STACK_SIZE * 2 )

/*-----------------------------------------------------------*/
/* Run-time stats and tracing. */
/*-----------------------------------------------------------*/

#define configUSE_TRACE_FACILITY                0
#define configGENERATE_RUN_TIME_STATS           0
#define configUSE_STATS_FORMATTING_FUNCTIONS    0

/*-----------------------------------------------------------*/
/* Hook function related definitions. */
/*-----------------------------------------------------------*/

#define configUSE_DAEMON_TASK_STARTUP_HOOK      0

/*-----------------------------------------------------------*/
/* Optional functions - most are enabled. */
/*-----------------------------------------------------------*/

#define INCLUDE_vTaskPrioritySet                1
#define INCLUDE_uxTaskPriorityGet               1
#define INCLUDE_vTaskDelete                     1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_xTaskGetSchedulerState          1
#define INCLUDE_xTaskGetCurrentTaskHandle       1
#define INCLUDE_xTaskGetIdleTaskHandle          1
#define INCLUDE_xTaskGetHandle                  1
#define INCLUDE_xTaskAbortDelay                 1
#define INCLUDE_xTaskGetTickCount               1
#define INCLUDE_uxTaskGetStackHighWaterMark     1
#define INCLUDE_uxTaskGetStackHighWaterMark2    1
#define INCLUDE_eTaskGetState                   1
#define INCLUDE_xTimerPendFunctionCall          1
#define INCLUDE_xQueueGetMutexHolder            1
#define INCLUDE_xSemaphoreGetMutexHolder        1
#define INCLUDE_xTaskResumeFromISR              1
#define INCLUDE_vTaskCleanUpResources           0

/*-----------------------------------------------------------*/
/* Assert handling. */
/*-----------------------------------------------------------*/

void vAssertCalled( const char * pcFile, int lLine );
#define configASSERT( x )                       if( ( x ) == 0 ) { vAssertCalled( __FILE__, __LINE__ ); }

/*-----------------------------------------------------------*/
/* Interrupt priority configuration (not used on RISC-V). */
/*-----------------------------------------------------------*/

#define configKERNEL_INTERRUPT_PRIORITY         ( 0 )
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    ( 0 )

/* Enable POSIX errno support for standard API compatibility. */
#define configUSE_POSIX_ERRNO                   1

/* Additional required kernel options. */
#ifndef configUSE_PASSIVE_IDLE_HOOK
    /* SMP 下建议启用被动 Idle Hook，以便在 Idle 期间执行平台维护工作（如软 tick）。 */
    #define configUSE_PASSIVE_IDLE_HOOK         1
#endif
#ifndef configUSE_TICK_HOOK
    #define configUSE_TICK_HOOK                 0
#endif

#endif /* FREERTOS_CONFIG_H */
