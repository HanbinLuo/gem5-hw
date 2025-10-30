
/*
 * FreeRTOS Kernel <DEVELOPMENT BRANCH>
 * QEMU RV32 virt SMP port.
 */

#include <stdint.h>
#include <string.h>

#include "FreeRTOS.h"
#include "portmacro.h"
#include "task.h"
#include "virt_config.h"

#ifndef configTASK_RETURN_ADDRESS
#define portTASK_RETURN_ADDRESS 0UL
#else
#define portTASK_RETURN_ADDRESS configTASK_RETURN_ADDRESS
#endif

#ifndef configUSE_ISR_STACK
#define configUSE_ISR_STACK 0
#endif

#ifndef portBYTE_ALIGNMENT_MASK
#define portBYTE_ALIGNMENT_MASK (portBYTE_ALIGNMENT - 1)
#endif

#define portCLINT_MSIP_OFFSET (0x0000UL)
#define portCLINT_MTIMECMP_OFFSET (0x4000UL)
#define portCLINT_MTIME_OFFSET (0xBFF8UL)

#define portMIE_MSIE_MASK (1UL << 3)
#define portMIE_MTIE_MASK (1UL << 7)

/*-----------------------------------------------------------*/

static inline volatile uint32_t* prvMsipAddress(UBaseType_t uxHart) {
  return (volatile uint32_t*)(configCLINT_BASE_ADDRESS + portCLINT_MSIP_OFFSET +
                              (uxHart * configCLINT_MSIP_STRIDE));
}

static inline volatile uint64_t* prvMtimecmpAddress(UBaseType_t uxHart) {
  return (volatile uint64_t*)(configCLINT_BASE_ADDRESS +
                              portCLINT_MTIMECMP_OFFSET +
                              (uxHart * configCLINT_MTIMER_STRIDE));
}

static inline uint64_t prvReadMTime(void) {
  volatile uint32_t* pulMTime =
      (volatile uint32_t*)(configCLINT_BASE_ADDRESS + portCLINT_MTIME_OFFSET);
  uint32_t ulHigh1;
  uint32_t ulLow;
  uint32_t ulHigh2;

  do {
    ulHigh1 = pulMTime[1];
    ulLow = pulMTime[0];
    ulHigh2 = pulMTime[1];
  } while (ulHigh1 != ulHigh2);

  return (((uint64_t)ulHigh1) << 32) | ulLow;
}

static inline void prvClearSoftwareInterrupt(UBaseType_t uxHart) {
  *prvMsipAddress(uxHart) = 0U;
}

static inline void prvTriggerSoftwareInterrupt(UBaseType_t uxHart) {
  *prvMsipAddress(uxHart) = 1U;
}

static inline void prvEnableSoftwareInterrupt(void) {
  __asm volatile("csrs mie, %0" ::"r"(portMIE_MSIE_MASK));
}

#ifdef DEBUG_UART
/* Minimal UART + CSR diagnostics for bring-up. */
static inline void dbg_uart_putc(char c) {
  volatile uint8_t* const uart = (volatile uint8_t*)0x10000000UL;
  if ((uart[5] & 0x20U) != 0U) {
    uart[0] = (uint8_t)c;
  }
}

static inline void dbg_uart_puts(const char* s) {
  while (*s) {
    dbg_uart_putc(*s++);
  }
}

static inline void dbg_uart_put_hex32(uint32_t v) {
  static const char hx[] = "0123456789ABCDEF";
  dbg_uart_puts("0x");
  for (int i = 7; i >= 0; --i) {
    dbg_uart_putc(hx[(v >> (i * 4)) & 0xF]);
  }
}

static inline uint32_t csr_read32(const char* name) {
  uint32_t v = 0;
  /* This helper is intentionally not generic due to inline asm constraints. */
  (void)name;
  return v;
}

static inline uint32_t read_mstatus(void) {
  uint32_t v;
  __asm volatile("csrr %0, mstatus" : "=r"(v));
  return v;
}
static inline uint32_t read_mie(void) {
  uint32_t v;
  __asm volatile("csrr %0, mie" : "=r"(v));
  return v;
}
static inline uint32_t read_mip(void) {
  uint32_t v;
  __asm volatile("csrr %0, mip" : "=r"(v));
  return v;
}
static inline uint32_t read_mtvec(void) {
  uint32_t v;
  __asm volatile("csrr %0, mtvec" : "=r"(v));
  return v;
}

static void dbg_dump_csrs(const char* tag) {
  dbg_uart_puts("\r\n[D] ");
  dbg_uart_puts(tag);
  dbg_uart_puts(" mtvec=");
  dbg_uart_put_hex32(read_mtvec());
  dbg_uart_puts(" mstatus=");
  dbg_uart_put_hex32(read_mstatus());
  dbg_uart_puts(" mie=");
  dbg_uart_put_hex32(read_mie());
  dbg_uart_puts(" mip=");
  dbg_uart_put_hex32(read_mip());
  /* Also print MSIP value for this hart. */
  UBaseType_t hart = (UBaseType_t)portGET_CORE_ID();
  uint32_t msip = *prvMsipAddress(hart);
  dbg_uart_puts(" msip=");
  dbg_uart_put_hex32(msip);
  dbg_uart_puts("\r\n");
}
#endif /* DEBUG_UART */

static inline uint64_t prvReadMCycle(void) {
#if (__riscv_xlen == 32)
  uint32_t hi1, lo, hi2;
  do {
    __asm volatile("csrr %0, mcycleh" : "=r"(hi1));
    __asm volatile("csrr %0, mcycle" : "=r"(lo));
    __asm volatile("csrr %0, mcycleh" : "=r"(hi2));
  } while (hi1 != hi2);
  return (((uint64_t)hi1) << 32) | lo;
#else
  uint64_t v;
  __asm volatile("csrr %0, mcycle" : "=r"(v));
  return v;
#endif
}

/*-----------------------------------------------------------*/

uint64_t ullNextTime = 0ULL;
const uint64_t* pullNextTime = &ullNextTime;
size_t uxTimerIncrementsForOneTick = 0U;
volatile uint64_t* pullMachineTimerCompareRegister = NULL;

typedef struct tskTaskControlBlock TCB_t;

#if (configNUMBER_OF_CORES == 1)
size_t xCriticalNesting = (size_t)0xaaaaaaaa;
size_t* xCriticalNestingArray = &xCriticalNesting;
extern PRIVILEGED_DATA TCB_t* volatile pxCurrentTCB;
TCB_t* volatile* pxCurrentTCBArray = &pxCurrentTCB;
#else
size_t xCriticalNestings[configNUMBER_OF_CORES] = {0};
size_t* xCriticalNestingArray = xCriticalNestings;
extern PRIVILEGED_DATA TCB_t* volatile pxCurrentTCBs[configNUMBER_OF_CORES];
TCB_t* volatile* pxCurrentTCBArray = pxCurrentTCBs;
#endif

StackType_t xISRStackTops[configNUMBER_OF_CORES] = {0};
size_t xTaskReturnAddress = (size_t)portTASK_RETURN_ADDRESS;

static uint8_t ucPrimaryCoreNum = 0xFFU;

/*-----------------------------------------------------------*/

static void prvProgramTimerCompare(uint64_t ullNext) {
  configASSERT(pullMachineTimerCompareRegister != NULL);
#if (__riscv_xlen == 32)
  volatile uint32_t* pulCompare =
      (volatile uint32_t*)pullMachineTimerCompareRegister;
  pulCompare[0] = 0xFFFFFFFFUL;
  pulCompare[1] = (uint32_t)(ullNext >> 32);
  pulCompare[0] = (uint32_t)ullNext;
#else
  *pullMachineTimerCompareRegister = ullNext;
#endif
}

void vPortSetupTimerInterrupt(void) {
  const UBaseType_t uxHart = (UBaseType_t)portGET_CORE_ID();

  if (uxHart != (UBaseType_t)configTICK_CORE) {
    return;
  }

  configASSERT(configCLINT_CLOCK_HZ != 0UL);
  configASSERT(configTICK_RATE_HZ != 0UL);

  uxTimerIncrementsForOneTick =
      (size_t)((configCLINT_CLOCK_HZ + (configTICK_RATE_HZ - 1UL)) /
               configTICK_RATE_HZ);
  configASSERT(uxTimerIncrementsForOneTick != 0U);

  pullMachineTimerCompareRegister = prvMtimecmpAddress(uxHart);

  const uint64_t ullCurrent = prvReadMTime();
  /* Program the first tick at the normal cadence to avoid an
     immediate pending MTIP before the first task is restored. */
  ullNextTime = ullCurrent + (uint64_t)uxTimerIncrementsForOneTick;
  prvProgramTimerCompare(ullNextTime);
  ullNextTime += uxTimerIncrementsForOneTick;

  __asm volatile("csrs mie, %0" ::"r"(portMIE_MTIE_MASK));
  prvEnableSoftwareInterrupt();

  /* Debug: mark timer setup on tick core and dump CSRs. */
#ifdef DEBUG_UART
  {
    volatile uint8_t* const uart = (volatile uint8_t*)0x10000000UL;
    if ((uart[5] & 0x20U) != 0U) {
      uart[0] = (uint8_t)'A';
    }
    dbg_dump_csrs("vPortSetupTimerInterrupt");
  }
#endif
}

void vPortInstallVectorTableHandlers(void) {
  /* 改为 Vectored 模式，指向我们在 portASM.S 中构建的向量表。 */
  extern void freertos_vector_table(void);
  uintptr_t base = (uintptr_t)freertos_vector_table;
  /* mtvec[1:0]=1 表示 Vectored，base 需对齐。 */
  uintptr_t mtvec_val = (base & ~((uintptr_t)0x3)) | 0x1u;
  __asm volatile("csrw mtvec, %0" ::"r"(mtvec_val));
#ifdef DEBUG_UART
  dbg_uart_puts("\r\n[D] vPortInstallVector base=");
  dbg_uart_put_hex32((uint32_t)base);
  dbg_dump_csrs("mtvec_set");
#endif
}

/* Forward declarations for ISR critical wrappers used by fallback. */
UBaseType_t callTaskEnterCriticalFromISR(void);
void callTaskExitCriticalFromISR(UBaseType_t uxSavedInterruptStatus);

/* Fallback: poll MTIMER compare and drive a tick from task context when
   the platform fails to deliver MTIP (e.g., some QEMU configurations).
   Safe to call frequently from the idle hook. */
void vPortTickPollFallback(void) {
#ifdef DEMO_FORCE_TICK_BOOST
  /* 强力自举：在早期(-bios none)环境下强制推进若干个节拍，
     避免依赖 mtime/mcycle 的可见性，迅速让系统进入稳态。 */
  static int32_t lBoost =
      (int32_t)DEMO_FORCE_TICK_BOOST; /* 例如 4000 表示先推 4 秒 @1kHz */
  if (lBoost > 0) {
    /* 每次最多推进 16 个，避免长时间占用 CPU。仅推进 tick，不在此处切换，
       由调用者或下方正常路径统一触发一次切换。 */
    int step = (lBoost > 16) ? 16 : (int)lBoost;
    for (int i = 0; i < step; i++) {
      UBaseType_t s = callTaskEnterCriticalFromISR();
      (void)xTaskIncrementTick();
      callTaskExitCriticalFromISR(s);
    }
    lBoost -= step;
#ifdef DEBUG_UART
    {
      volatile uint8_t* const uart = (volatile uint8_t*)0x10000000UL;
      if ((uart[5] & 0x20U) != 0U) {
        uart[0] = (uint8_t)'f';
      }
    }
#endif
    /* 不 return，继续走正常路径，由正常路径一次性触发切换。 */
  }
#endif
  /* Debug入口标记，确认被调用（仅在 DebugUart=ON 时可见）。 */
#ifdef DEBUG_UART
  {
    volatile uint8_t* const uart = (volatile uint8_t*)0x10000000UL;
    if ((uart[5] & 0x20U) != 0U) {
      uart[0] = (uint8_t)'b';
    }
  }
#endif
  /* 仅在 tick 核上执行软 tick，以避免多核并发导致的竞争和潜在自旋锁争用。 */
  if ((UBaseType_t)portGET_CORE_ID() != (UBaseType_t)configTICK_CORE) {
    return;
  }

  if (pullMachineTimerCompareRegister == NULL) {
    /* Lazy init on tick core if the timer setup didn't run yet. */
    const UBaseType_t uxHart = (UBaseType_t)portGET_CORE_ID();
    if (uxHart == (UBaseType_t)configTICK_CORE) {
      if (uxTimerIncrementsForOneTick == 0U) {
        uxTimerIncrementsForOneTick =
            (size_t)((configCLINT_CLOCK_HZ + (configTICK_RATE_HZ - 1UL)) /
                     configTICK_RATE_HZ);
        if (uxTimerIncrementsForOneTick == 0U) {
          return;
        }
      }
      pullMachineTimerCompareRegister = prvMtimecmpAddress(uxHart);
      uint64_t now0 = prvReadMTime();
      ullNextTime = now0 + (uint64_t)uxTimerIncrementsForOneTick;
      prvProgramTimerCompare(ullNextTime);
      ullNextTime += uxTimerIncrementsForOneTick;
    } else {
      return;
    }
  }

  static uint64_t ullLastMTime = 0ULL;
  static uint32_t ulNoProgressLoops = 0U;
  const uint32_t ulNoProgressThreshold = 8U; /* 更敏感：尽快触发 mcycle 兜底 */

  /* cycle 兜底：按 CPU 周期近似驱动节拍，避免依赖 mtime。 */
  static uint64_t ullLastCycle = 0ULL;
  static uint64_t ullCycleResidue = 0ULL;
  const uint64_t ullCyclesPerTick =
      ((uint64_t)configCPU_CLOCK_HZ) / (uint64_t)configTICK_RATE_HZ;

  uint64_t now = prvReadMTime();
  /* 使用影子比较值，避免 RV32 读取 64 位 MMIO 不一致。 */
  uint64_t cmp_shadow = ullNextTime - (uint64_t)uxTimerIncrementsForOneTick;

  /* 为防止错过多个节拍，这里允许一次 Idle 循环内追赶至多 8 个 tick。 */
  BaseType_t xDoSwitch = pdFALSE;
  int loop = 0;
  while ((int64_t)(now - cmp_shadow) >= 0) {
    prvProgramTimerCompare(ullNextTime);
    ullNextTime += uxTimerIncrementsForOneTick;
    cmp_shadow += uxTimerIncrementsForOneTick;

    UBaseType_t s = callTaskEnterCriticalFromISR();
    BaseType_t x = xTaskIncrementTick();
    callTaskExitCriticalFromISR(s);
    if (x != pdFALSE) {
      xDoSwitch = pdTRUE;
    }

#ifdef DEBUG_UART
    {
      volatile uint8_t* const uart = (volatile uint8_t*)0x10000000UL;
      if ((uart[5] & 0x20U) != 0U) {
        uart[0] = (uint8_t)'F';
      }
    }
#endif

    if (++loop >= 8) {
      break;
    }
    now = prvReadMTime();
  }

  /* 若检测到 mtime 长期未前进，则退化为纯软件 tick，保证系统前进。 */
  if (now == ullLastMTime) {
    if (ulNoProgressLoops < 0xFFFFFFFFU) {
      ulNoProgressLoops++;
    }
  } else {
    ulNoProgressLoops = 0U;
    ullLastMTime = now;
  }

  if (ulNoProgressLoops >= ulNoProgressThreshold) {
    /* 使用 mcycle 驱动多个节拍（如果可用）。 */
    uint64_t cyc = prvReadMCycle();
    if (ullLastCycle == 0ULL) {
      ullLastCycle = cyc;
    }
    uint64_t dcyc = cyc - ullLastCycle;
    ullLastCycle = cyc;
    ullCycleResidue += dcyc;

    BaseType_t xDoSwitch2 = pdFALSE;
    /* 最多追赶 16 个节拍，防止一次性推进过多。 */
    for (int k = 0; k < 16; k++) {
      if (ullCycleResidue < ullCyclesPerTick || ullCyclesPerTick == 0ULL) {
        break;
      }
      ullCycleResidue -= ullCyclesPerTick;
      UBaseType_t s2 = callTaskEnterCriticalFromISR();
      BaseType_t x2 = xTaskIncrementTick();
      callTaskExitCriticalFromISR(s2);
      if (x2 != pdFALSE) {
        xDoSwitch2 = pdTRUE;
      }
    }

    ulNoProgressLoops = 0U;
    if (xDoSwitch2 != pdFALSE) {
      portYIELD_FROM_ISR(pdTRUE);
    }
#ifdef DEBUG_UART
    {
      volatile uint8_t* const uart = (volatile uint8_t*)0x10000000UL;
      if ((uart[5] & 0x20U) != 0U) {
        uart[0] = (uint8_t)'f';
      }
    }
#endif
  }

  if (xDoSwitch != pdFALSE) {
    portYIELD_FROM_ISR(pdTRUE);
  }
}

static StackType_t prvInitISRStackForCore(void) {
#if (configUSE_ISR_STACK == 1)
  StackType_t xTop = 0;
  xISRStackTops[portGET_CORE_ID()] = xTop;
  return xTop;
#else
  xISRStackTops[portGET_CORE_ID()] = 0;
  return 0;
#endif
}

static BaseType_t prvStartSchedulerOnCore(BaseType_t xIsPrimary) {
  extern void xPortStartFirstTask(void);
#if (configNUMBER_OF_CORES == 1)
  void vTaskSwitchContext(void);
#else
  void vTaskSwitchContext(BaseType_t xCoreID);
#endif

  (void)prvInitISRStackForCore();

  prvEnableSoftwareInterrupt();

  /* No early UART debug here to avoid timing side-effects. */

  if (xIsPrimary != pdFALSE) {
#ifndef DEMO_SOFT_TICK
    vPortSetupTimerInterrupt();
#endif
  }

  /* Do not enable MIE here; it will be enabled in xPortStartFirstTask
     after the first task's context (SP/mstatus) is loaded, ensuring
     trap context save has a valid current TCB. */

  /* Defer first context switch to after first task loads and interrupts enable.
   */
#ifdef DEBUG_UART
  {
    volatile uint8_t* const uart = (volatile uint8_t*)0x10000000UL;
    if ((uart[5] & 0x20U) != 0U) {
      uart[0] = (uint8_t)'B';
    }
  }
#endif
  xPortStartFirstTask();

  /* Should not return. */
  return pdFAIL;
}

BaseType_t xPortStartScheduler(void) {
  const UBaseType_t uxHart = (UBaseType_t)portGET_CORE_ID();

  configASSERT(ucPrimaryCoreNum == 0xFFU);
  ucPrimaryCoreNum = (uint8_t)uxHart;

  vPortInstallVectorTableHandlers();

  for (UBaseType_t ux = 0; ux < configNUMBER_OF_CORES; ux++) {
    prvClearSoftwareInterrupt(ux);
  }

  return prvStartSchedulerOnCore(pdTRUE);
}

void vPortStartSchedulerOnSecondaryCore(void) {
  /* 等待主核先调用 xPortStartScheduler 完成基本初始化，
     包括设置 ucPrimaryCoreNum 和清理全局 MSIP 等。 */
  while (ucPrimaryCoreNum == 0xFFU) {
    __asm volatile("nop" ::: "memory");
  }

  /* 每个 hart 都需要设置自己的 mtvec。 */
  vPortInstallVectorTableHandlers();

  (void)prvStartSchedulerOnCore(pdFALSE);
}

void vPortEndScheduler(void) {
  for (;;) {
  }
}

/*-----------------------------------------------------------*/

__attribute__((weak)) BaseType_t xPortHandleExternalInterrupt(uint32_t ulMcause,
                                                              uint32_t ulMepc) {
  (void)ulMcause;
  (void)ulMepc;
  return pdFALSE;
}

void freertos_risc_v_application_interrupt_handler(uint32_t ulMcause,
                                                   uint32_t ulMepc) {
  const UBaseType_t uxHart = (UBaseType_t)portGET_CORE_ID();
  const uint32_t ulCauseCode = ulMcause & ~(1UL << (__riscv_xlen - 1));
  BaseType_t xYieldRequired = pdFALSE;

  switch (ulCauseCode) {
    case 3UL: /* Machine software interrupt. */
      prvClearSoftwareInterrupt(uxHart);
      /* 兼容某些实现：同时清 mip.MSIP 挂起位。 */
      __asm volatile("csrc mip, %0" ::"r"((uint32_t)(1UL << 3)));
      xYieldRequired = pdTRUE;
      break;

    case 11UL: /* Machine external interrupt. */
      xYieldRequired = xPortHandleExternalInterrupt(ulMcause, ulMepc);
      break;

    default:
      (void)xPortHandleExternalInterrupt(ulMcause, ulMepc);
      break;
  }

  if (xYieldRequired != pdFALSE) {
    portYIELD_FROM_ISR(pdTRUE);
  }
}

/*-----------------------------------------------------------*/

#if (configNUMBER_OF_CORES != 1)
static volatile uint32_t ulSpinLocks[portRTOS_SPINLOCK_COUNT] = {0U};
static volatile uint8_t ucOwnedByCore[portMAX_CORE_COUNT]
                                     [portRTOS_SPINLOCK_COUNT] = {0U};
static volatile uint8_t ucRecursionCount[portRTOS_SPINLOCK_COUNT] = {0U};

void vPortRecursiveLock(BaseType_t xCoreID, uint32_t ulLockNum,
                        BaseType_t xAcquire) {
  configASSERT((xCoreID >= 0) &&
               ((UBaseType_t)xCoreID < configNUMBER_OF_CORES));
  configASSERT(ulLockNum < portRTOS_SPINLOCK_COUNT);

  if (xAcquire != pdFALSE) {
    if (ucOwnedByCore[xCoreID][ulLockNum] != 0U) {
      configASSERT(ucRecursionCount[ulLockNum] != 0U);
      ucRecursionCount[ulLockNum]++;
      return;
    }

    while (__atomic_exchange_n(&ulSpinLocks[ulLockNum], 1U, __ATOMIC_ACQUIRE) !=
           0U) {
      __asm volatile("nop" ::: "memory");
    }

    ucOwnedByCore[xCoreID][ulLockNum] = 1U;
    ucRecursionCount[ulLockNum] = 1U;
  } else {
    configASSERT(ucOwnedByCore[xCoreID][ulLockNum] != 0U);
    configASSERT(ucRecursionCount[ulLockNum] != 0U);

    if (--ucRecursionCount[ulLockNum] == 0U) {
      ucOwnedByCore[xCoreID][ulLockNum] = 0U;
      __atomic_store_n(&ulSpinLocks[ulLockNum], 0U, __ATOMIC_RELEASE);
    }
  }
}
#endif

/*-----------------------------------------------------------*/

void vYieldCore(int xCoreID) {
#if (configNUMBER_OF_CORES != 1)
  configASSERT(xCoreID >= 0);
  configASSERT(xCoreID < (int)configNUMBER_OF_CORES);
  if (xCoreID == (int)portGET_CORE_ID()) {
    /* If yielding the current core, trigger via exception path to
       guarantee an immediate schedule even if MSIP delivery lags. */
    __asm volatile("ecall");
  } else {
    prvTriggerSoftwareInterrupt((UBaseType_t)xCoreID);
  }
#else
  (void)xCoreID;
#endif
}

/*-----------------------------------------------------------*/

void vPortYieldSelf(void) {
  /* 打开 MSIE/MIE，确保软件中断可被接受。 */
  __asm volatile("csrs mie, %0" ::"r"(portMIE_MSIE_MASK));
  __asm volatile("csrs mstatus, %0" ::"r"((uint32_t)(1UL << 3)));
  __asm volatile("fence iorw, iorw" ::: "memory");

  /* 触发本核 MSIP，以通过中断路径完成一次让出/调度。 */
  UBaseType_t hart = (UBaseType_t)portGET_CORE_ID();
  volatile uint32_t* pMsip = prvMsipAddress(hart);
  for (int i = 0; i < 4; i++) {
    *pMsip = 1U;
    __asm volatile("fence iorw, iorw" ::: "memory");
    if (*pMsip != 0U) {
      break;
    }
  }
  /* Fallback: 如果 MSIP MMIO 不生效，直接置 mip.MSIP 挂起位。 */
  if (*pMsip == 0U) {
    const uint32_t msip_mask = (1UL << 3);
    __asm volatile("csrs mip, %0" ::"r"(msip_mask));
  }
  __asm volatile("fence iorw, iorw" ::: "memory");
#ifdef DEBUG_UART
  dbg_dump_csrs("vPortYieldSelf(after MSIP)");
#endif
}

/*-----------------------------------------------------------*/

UBaseType_t callTaskEnterCriticalFromISR(void) {
#if (configNUMBER_OF_CORES > 1)
  return taskENTER_CRITICAL_FROM_ISR();
#else
  return 0;
#endif
}

void callTaskExitCriticalFromISR(UBaseType_t uxSavedInterruptStatus) {
#if (configNUMBER_OF_CORES > 1)
  taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);
#else
  (void)uxSavedInterruptStatus;
#endif
}
