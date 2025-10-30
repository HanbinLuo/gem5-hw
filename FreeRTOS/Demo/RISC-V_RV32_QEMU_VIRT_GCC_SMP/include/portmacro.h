/*
 * FreeRTOS Kernel <DEVELOPMENT BRANCH>
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef PORTMACRO_H
#define PORTMACRO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

#include "virt_config.h"

/*-----------------------------------------------------------*/
/* Port specific definitions. */
/*-----------------------------------------------------------*/

#if __riscv_xlen == 64
    #define portSTACK_TYPE           uint64_t
    #define portBASE_TYPE            int64_t
    #define portUBASE_TYPE           uint64_t
    #define portMAX_DELAY            ( TickType_t ) 0xffffffffffffffffULL
    #define portPOINTER_SIZE_TYPE    uint64_t
#elif __riscv_xlen == 32
    #define portSTACK_TYPE           uint32_t
    #define portBASE_TYPE            long
    #define portUBASE_TYPE           unsigned long
    #define portMAX_DELAY            ( TickType_t ) 0xffffffffUL
#else
    #error "Unsupported RISC-V XLEN"
#endif

typedef portSTACK_TYPE   StackType_t;
typedef portBASE_TYPE    BaseType_t;
typedef portUBASE_TYPE   UBaseType_t;
typedef portUBASE_TYPE   TickType_t;

void vPortStartSchedulerOnSecondaryCore( void );

#define portCHAR     char
#define portFLOAT    float
#define portDOUBLE   double
#define portLONG     long
#define portSHORT    short

#define portTICK_TYPE_IS_ATOMIC    1

#define portSTACK_GROWTH          ( -1 )
#define portTICK_PERIOD_MS        ( ( TickType_t ) 1000 / configTICK_RATE_HZ )
#ifdef __riscv_32e
    #define portBYTE_ALIGNMENT    8
#else
    #define portBYTE_ALIGNMENT    16
#endif

#ifndef portMAX_CORE_COUNT
    #define portMAX_CORE_COUNT    4
#endif

#if ( configNUMBER_OF_CORES < 1 ) || ( configNUMBER_OF_CORES > portMAX_CORE_COUNT )
    #error "configNUMBER_OF_CORES out of supported range"
#endif

#if ( configNUMBER_OF_CORES > 1 )
    #ifndef configTICK_CORE
        #define configTICK_CORE    0
    #endif
    #if ( configTICK_CORE < 0 ) || ( configTICK_CORE >= configNUMBER_OF_CORES )
        #error "configTICK_CORE must select a valid hart"
    #endif
#endif

static inline UBaseType_t portGetHartId( void )
{
    uintptr_t uxHartId;
    __asm volatile( "csrr %0, mhartid" : "=r" ( uxHartId ) );
    return ( UBaseType_t ) uxHartId;
}
#define portGET_CORE_ID()    ( ( BaseType_t ) portGetHartId() )

static inline BaseType_t portCHECK_IF_IN_ISR( void )
{
    uintptr_t uxMcause;
    __asm volatile( "csrr %0, mcause" : "=r" ( uxMcause ) );
    return ( BaseType_t ) ( uxMcause >> ( __riscv_xlen - 1 ) );
}

void vYieldCore( int xCoreID );
#define portYIELD_CORE( a )                  vYieldCore( a )

void vPortYieldSelf( void );
#ifdef DEBUG_UART
static inline void portdbg_putc_inline( char c )
{
    volatile uint8_t * const uart = ( volatile uint8_t * ) 0x10000000UL;
    if( ( uart[ 5 ] & 0x20U ) != 0U ) { uart[ 0 ] = ( uint8_t ) c; }
}
#define portYIELD()                do { portdbg_putc_inline('Y'); vPortYieldSelf(); } while(0)
#else
#define portYIELD()                vPortYieldSelf()
#endif

/* 在当前 QEMU 环境（mtvec=0）下，ecall 不会进入 trap，
   为保证从任务 API 中的让出能立即切换到 Idle（从而让软 tick 推进），
   将 portYIELD_WITHIN_API 改为直接执行一次任务级切换。*/
#undef portYIELD_WITHIN_API
/* 恢复 API 让出语义为触发一次让出（由陷阱/中断路径处理）。 */
#define portYIELD_WITHIN_API()     portYIELD()

#if configNUMBER_OF_CORES == 1
    #define portTASK_SWITCH_CONTEXT()    vTaskSwitchContext()
#else
    #define portTASK_SWITCH_CONTEXT()    vTaskSwitchContext( portGET_CORE_ID() )
#endif

#define portEND_SWITCHING_ISR( xSwitchRequired )    \
    do                                              \
    {                                               \
        if( ( xSwitchRequired ) != pdFALSE )        \
        {                                           \
            traceISR_EXIT_TO_SCHEDULER();           \
            portTASK_SWITCH_CONTEXT();              \
        }                                           \
        else                                        \
        {                                           \
            traceISR_EXIT();                        \
        }                                           \
    } while( 0 )

#define portYIELD_FROM_ISR( x )    portEND_SWITCHING_ISR( x )

#define portMSTATUS_MIE    ( 1UL << 3 )

static inline UBaseType_t portDisableInterrupts( void )
{
    UBaseType_t uxOriginal;
    __asm volatile( "csrrc %0, mstatus, %1" : "=r" ( uxOriginal ) : "r" ( portMSTATUS_MIE ) : "memory" );
    return uxOriginal;
}

static inline void portRestoreInterrupts( UBaseType_t uxState )
{
    __asm volatile( "csrw mstatus, %0" :: "r" ( uxState ) : "memory" );
}

static inline void portEnableInterrupts( void )
{
    __asm volatile( "csrs mstatus, %0" :: "r" ( portMSTATUS_MIE ) : "memory" );
}

#define portSET_INTERRUPT_MASK()             portDisableInterrupts()
#define portCLEAR_INTERRUPT_MASK( state )    portRestoreInterrupts( state )
#define portDISABLE_INTERRUPTS()             ( void ) portDisableInterrupts()
#define portENABLE_INTERRUPTS()              portEnableInterrupts()

#if ( configNUMBER_OF_CORES == 1 )
extern size_t xCriticalNesting;
#define portGET_CRITICAL_NESTING_COUNT()             ( xCriticalNesting )
#define portENTER_CRITICAL()                         \
    do                                               \
    {                                                \
        portDISABLE_INTERRUPTS();                    \
        xCriticalNesting++;                          \
    } while( 0 )
#define portEXIT_CRITICAL()                          \
    do                                               \
    {                                                \
        configASSERT( xCriticalNesting > 0 );        \
        xCriticalNesting--;                          \
        if( xCriticalNesting == 0U )                 \
        {                                            \
            portENABLE_INTERRUPTS();                 \
        }                                            \
    } while( 0 )
#else
extern size_t xCriticalNestings[ configNUMBER_OF_CORES ];
void vTaskEnterCritical( void );
void vTaskExitCritical( void );
UBaseType_t vTaskEnterCriticalFromISR( void );
void vTaskExitCriticalFromISR( UBaseType_t uxSavedInterruptStatus );
#define portENTER_CRITICAL()               vTaskEnterCritical()
#define portEXIT_CRITICAL()                vTaskExitCritical()
#define portENTER_CRITICAL_FROM_ISR()      vTaskEnterCriticalFromISR()
#define portEXIT_CRITICAL_FROM_ISR( x )    vTaskExitCriticalFromISR( x )
#define portGET_CRITICAL_NESTING_COUNT()          ( xCriticalNestings[ portGET_CORE_ID() ] )
#define portSET_CRITICAL_NESTING_COUNT( x )       ( xCriticalNestings[ portGET_CORE_ID() ] = ( x ) )
#define portINCREMENT_CRITICAL_NESTING_COUNT()    ( xCriticalNestings[ portGET_CORE_ID() ]++ )
#define portDECREMENT_CRITICAL_NESTING_COUNT()    ( xCriticalNestings[ portGET_CORE_ID() ]-- )
#endif

#ifndef configUSE_PORT_OPTIMISED_TASK_SELECTION
    #if ( configNUMBER_OF_CORES == 1 )
        #define configUSE_PORT_OPTIMISED_TASK_SELECTION    1
    #endif
#endif

#if ( configUSE_PORT_OPTIMISED_TASK_SELECTION == 1 )
    #if ( configMAX_PRIORITIES > 32 )
        #error "configUSE_PORT_OPTIMISED_TASK_SELECTION requires configMAX_PRIORITIES <= 32"
    #endif
    #define portRECORD_READY_PRIORITY( uxPriority, uxReadyPriorities )    ( uxReadyPriorities ) |= ( 1UL << ( uxPriority ) )
    #define portRESET_READY_PRIORITY( uxPriority, uxReadyPriorities )     ( uxReadyPriorities ) &= ~( 1UL << ( uxPriority ) )
    #define portGET_HIGHEST_PRIORITY( uxTopPriority, uxReadyPriorities )  uxTopPriority = ( 31UL - __builtin_clz( uxReadyPriorities ) )
#endif

#define portTASK_FUNCTION_PROTO( vFunction, pvParameters )    void vFunction( void * pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters )          void vFunction( void * pvParameters )

#define portNOP()                 __asm volatile ( "nop" )
#define portINLINE                __inline
#ifndef portFORCE_INLINE
    #define portFORCE_INLINE      inline __attribute__( ( always_inline ) )
#endif
#define portMEMORY_BARRIER()      __asm volatile ( "" ::: "memory" )

#define portCRITICAL_NESTING_IN_TCB    0
#define portRTOS_SPINLOCK_COUNT        2

#if ( configNUMBER_OF_CORES == 1 )
    #define portGET_ISR_LOCK()
    #define portRELEASE_ISR_LOCK()
    #define portGET_TASK_LOCK()
    #define portRELEASE_TASK_LOCK()
#else
void vPortRecursiveLock( BaseType_t xCoreID, uint32_t ulLockNum, BaseType_t xAcquire );
    #define portGET_ISR_LOCK()         vPortRecursiveLock( portGET_CORE_ID(), 0u, pdTRUE )
    #define portRELEASE_ISR_LOCK()     vPortRecursiveLock( portGET_CORE_ID(), 0u, pdFALSE )
    #define portGET_TASK_LOCK()        vPortRecursiveLock( portGET_CORE_ID(), 1u, pdTRUE )
    #define portRELEASE_TASK_LOCK()    vPortRecursiveLock( portGET_CORE_ID(), 1u, pdFALSE )
#endif

#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */


