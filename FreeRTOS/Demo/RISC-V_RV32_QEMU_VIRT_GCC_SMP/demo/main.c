#include <stddef.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "plic_handler.h"
#include "portmacro.h"
#include "queue.h"
#include "task.h"
#include "uart16550.h"

#define STACK_CHECK_VALUE 0xDEADBEEFU

extern UBaseType_t callTaskEnterCriticalFromISR(void);
extern void callTaskExitCriticalFromISR(UBaseType_t uxSavedInterruptStatus);
extern void vPortInstallVectorTableHandlers(void);
extern void vPortSetupTimerInterrupt(void);
extern void vStartDagDemo(void);

extern size_t strlen(const char* pcString);

static void prvPrimaryCoreInit(void);
static void prvHeartbeatTask(void* pvParameters);
// static void prvPlicSmokeTestTask(void* pvParameters);

static volatile uint32_t ulSecondaryRelease = 0U;
static TaskHandle_t xHeartbeatHandle = NULL;

/* Wake up secondary harts on QEMU -bios none by asserting CLINT MSIP. */
static void prvWakeSecondaryHarts(void) {
  for (UBaseType_t uxHart = 1U; uxHart < (UBaseType_t)configNUMBER_OF_CORES;
       uxHart++) {
    volatile uint32_t* const pulMsip =
        (volatile uint32_t*)(configCLINT_BASE_ADDRESS + 0x0000UL +
                             (uxHart * configCLINT_MSIP_STRIDE));
    *pulMsip = 1U;
  }
  __asm volatile("fence w, rw" ::: "memory");
}

/*-----------------------------------------------------------*/

int main(void) {
  const UBaseType_t uxHart = (UBaseType_t)portGET_CORE_ID();

  if (uxHart == (UBaseType_t)configTICK_CORE) {
    prvPrimaryCoreInit();

    vTaskStartScheduler();
    /* Should never reach here. If we do, log for diagnostics. */
    LOGF("scheduler_returned\n");
  } else {
#ifdef DEMO_PARK_SECONDARY
    /* -bios none 下为兼容性考虑：不在次核启动调度，避免在无中断环境触发断言。
     */
    LOGF("[core secondary] parked\n");
    for (;;) {
      __asm volatile("nop" ::: "memory");
    }
#else
    while (__atomic_load_n(&ulSecondaryRelease, __ATOMIC_ACQUIRE) == 0U) {
      __asm volatile("nop" ::: "memory");
    }
    vPortInstallVectorTableHandlers(); /* 次核 mtvec */
    LOGF("[core secondary] entering scheduler\n");
    vPortStartSchedulerOnSecondaryCore();
#endif
  }

  for (;;) {
    __asm volatile("wfi" ::: "memory");
  }
}

/*-----------------------------------------------------------*/

static void prvPrimaryCoreInit(void) {
  /* 1) 安装向量表 / SysTick，这部分保持你原来的 */
  vPortInstallVectorTableHandlers(); /* 设置 mtvec -> FreeRTOS 向量表 */
#ifndef DEMO_SOFT_TICK
  vPortSetupTimerInterrupt(); /* 预置 mtimecmp + 开 mie.MTIE（仅 tick 核有效）*/
#endif

  /* 2) 初始化 Uart */
  prvUartInit();
  LOGF("FreeRTOS SMP demo (QEMU RV32 virt)\n");

  /* 3) 使能机器外部中断 */
  vEnableMachineExternalInterrupts();

  /* 4) 初始化 PLIC（使能 CU0 的中断） */
  vPlicInit();

  if (xTaskCreate(prvHeartbeatTask, "pulse", configMINIMAL_STACK_SIZE + 128U,
                  NULL, tskIDLE_PRIORITY + 3U, &xHeartbeatHandle) != pdPASS) {
    LOGF("heartbeat_create_failed\n");
  }
  /* 固定亲和性：心跳跑在核0 */
  if (xHeartbeatHandle != NULL) {
    vTaskCoreAffinitySet(xHeartbeatHandle, (1U << 0));
  }

  // if (xTaskCreate(prvPlicSmokeTestTask, "plic", configMINIMAL_STACK_SIZE +
  // 128u,
  //                 NULL, tskIDLE_PRIORITY + 4u, &xPlicTestHandle) != pdPASS) {
  //   vDemoLogString("[plic] task create failed\n");
  // }
  // /* PLIC 中断测试程序固定在 core0 */
  // if (xPlicTestHandle != NULL) {
  //   vTaskCoreAffinitySet(xPlicTestHandle, (1u << 0));
  // }

  /* 启动 DAG 运行时：初始化 DAG + 创建 DAG worker 任务 */
  vStartDagDemo();
}

/*-----------------------------------------------------------*/

static void prvHeartbeatTask(void* pvParameters) {
  (void)pvParameters;

  /* 只跑一次：唤醒所有 secondary 核 */
  LOGF("[heartbeat] releasing secondaries\n");

  prvWakeSecondaryHarts();
  __atomic_store_n(&ulSecondaryRelease, 1U, __ATOMIC_RELEASE);
  __asm volatile("fence w, rw" ::: "memory");

  LOGF("[heartbeat] secondaries released\n");

  /* 做一点点防御性延时，保证日志打出去（可选） */
  // vTaskDelay(pdMS_TO_TICKS(1));

  LOGF("[heartbeat] exit\n");

  /* 自杀，释放 core0 调度给真正的业务任务 */
  vTaskDelete(NULL);

  /* 永远不会走到这里 */
  for (;;) {
  }
}

/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook(void) {
  taskDISABLE_INTERRUPTS();
  LOGF("malloc_failed\n");
  for (;;) {
  }
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName) {
  (void)xTask;
  LOGF("%s\n", pcTaskName);
  LOGF("stack_overflow\n");
  taskDISABLE_INTERRUPTS();
  for (;;) {
  }
}

void vAssertCalled(const char* pcFile, int lLine) {
  (void)pcFile;
  LOGF("FILE[%s] LINE[%d]\n", pcFile, (uint32_t)lLine);
  LOGF("assert\n");
  taskDISABLE_INTERRUPTS();
  for (;;) {
  }
}

void vApplicationIdleHook(void) { __asm volatile("nop"); }

void vApplicationPassiveIdleHook(void) {
#ifdef DEMO_SOFT_TICK
  extern void vPortTickPollFallback(void);
  vPortTickPollFallback();
#endif
}

// static void prvPlicSmokeTestTask(void* pv) {
//   (void)pv;

//   /* 1) PLIC priority */
//   mmio_write32(PLIC_BASE + 4u * PLIC_IRQ, 1u);

//   /* 2) Enable for context 0 */
//   uint32_t enable_addr =
//       PLIC_BASE + 0x2000u + (PLIC_CONTEXT * 0x80u) + ((PLIC_IRQ >> 5) * 4u);
//   uint32_t val = mmio_read32(enable_addr);
//   val |= (1u << (PLIC_IRQ & 0x1Fu));
//   mmio_write32(enable_addr, val);

//   /* 3) Threshold */
//   mmio_write32(PLIC_BASE + 0x0200000u + (PLIC_CONTEXT * 0x1000u), 0u);

//   /* 4) Enable MEIE without clobbering MTIE etc */
//   __asm__ volatile("csrs mie, %0" ::"r"(1u << 11));
//   __asm__ volatile("csrs mstatus, %0" ::"r"(1u << 3));

//   /* 5) Wait for IRQ */
//   uint32_t got = ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(1000u));
//   if (got != 0u) {
//     vDemoLogString("[plic] irq observed\n");
//   } else {
//     vDemoLogString("[plic] timeout\n");
//   }

//   vTaskDelete(NULL);
// }

/*-----------------------------------------------------------*/
