#include <stddef.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "portmacro.h"
#include "queue.h"
#include "task.h"
extern UBaseType_t callTaskEnterCriticalFromISR(void);
extern void callTaskExitCriticalFromISR(UBaseType_t uxSavedInterruptStatus);
extern void vPortInstallVectorTableHandlers(void);
extern void vPortSetupTimerInterrupt(void);

extern size_t strlen(const char* pcString);

extern void vStartDagDemo(void);

#define UART0_BASE 0x10000000UL
#define UART_REG_DLL 0x00
#define UART_REG_DLM 0x01
#define UART_REG_FCR 0x02
#define UART_REG_LCR 0x03
#define UART_REG_MCR 0x04
#define UART_REG_LSR 0x05
#define UART_REG_THR 0x00
#define UART_REG_RHR 0x00
#define UART_CLOCK_HZ 1843200UL
#define UART_BAUD 115200UL

#define UART_LCR_DLAB 0x80U
#define UART_LCR_8N1 0x03U
#define UART_FCR_ENABLE 0x01U
#define UART_FCR_CLEAR_RX 0x02U
#define UART_FCR_CLEAR_TX 0x04U
#define UART_LSR_TX_EMPTY 0x20U

#define STACK_CHECK_VALUE 0xDEADBEEFU

static void prvPrimaryCoreInit(void);
static void prvHeartbeatTask(void* pvParameters);
static void prvWorkerTask(void* pvParameters);
static void prvUartInit(void);
static void prvUartPutRaw(char c);
static void prvUartWrite(const char* pcData, size_t xLength);
static void vDemoLogDecimal(const char* pcLabel, uint32_t ulValue);
static void vDemoLogHexPair(const char* pcLabel, uint32_t ulFirst,
                            uint32_t ulSecond);
static size_t prvWriteString(char* pcDest, const char* pcSrc);
static size_t prvWriteDecimal(char* pcDest, uint32_t ulValue);
static size_t prvWriteHex(char* pcDest, uint32_t ulValue);
// static uint32_t prvBusyChecksum(uint32_t ulSeed);
static void vDemoLogString(const char* pcMsg);

static volatile uint32_t ulSecondaryRelease = 0U;
static TaskHandle_t xWorkerHandle = NULL;
static TaskHandle_t xHeartbeatHandle = NULL;

static QueueHandle_t xChan = NULL;

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

    // TickType_t xNext = xTaskGetTickCount();
    // xNext += pdMS_TO_TICKS(1U);
    // vTaskDelayUntil(&xNext, pdMS_TO_TICKS(1U));

    vTaskStartScheduler();
    /* Should never reach here. If we do, log for diagnostics. */
    vDemoLogString("scheduler_returned\n");
  } else {
#ifdef DEMO_PARK_SECONDARY
    /* -bios none 下为兼容性考虑：不在次核启动调度，避免在无中断环境触发断言。
     */
    vDemoLogString("[core secondary] parked\n");
    for (;;) {
      __asm volatile("nop" ::: "memory");
    }
#else
    while (__atomic_load_n(&ulSecondaryRelease, __ATOMIC_ACQUIRE) == 0U) {
      __asm volatile("nop" ::: "memory");
    }
    vPortInstallVectorTableHandlers(); /* 次核 mtvec */
    vDemoLogString("[core secondary] entering scheduler\n");
    vPortStartSchedulerOnSecondaryCore();
#endif
  }

  for (;;) {
    __asm volatile("wfi" ::: "memory");
  }
}

/*-----------------------------------------------------------*/

static void prvPrimaryCoreInit(void) {
  prvUartInit();
  vPortInstallVectorTableHandlers(); /* 设置 mtvec -> FreeRTOS 向量表 */
#ifndef DEMO_SOFT_TICK
  vPortSetupTimerInterrupt(); /* 预置 mtimecmp + 开 mie.MTIE（仅 tick 核有效）*/
#endif
  vDemoLogString("FreeRTOS SMP demo (QEMU RV32 virt)\n");

  // /* 创建通信通道 */
  // xChan = xQueueCreate(16, sizeof(uint32_t));
  // if (xChan == NULL) {
  //   vDemoLogDecimal("queue_create_failed", 0U);
  // }

  if (xTaskCreate(prvHeartbeatTask, "pulse", configMINIMAL_STACK_SIZE + 128U,
                  NULL, tskIDLE_PRIORITY + 3U, &xHeartbeatHandle) != pdPASS) {
    vDemoLogDecimal("heartbeat_create_failed", 0U);
  }

  // /* 创建 worker 任务以演示 SMP；软 tick/Idle 切换将推动两核调度。 */
  // if (xTaskCreate(prvWorkerTask, "worker", configMINIMAL_STACK_SIZE + 256U,
  //                 NULL, tskIDLE_PRIORITY + 1U, &xWorkerHandle) != pdPASS) {
  //   vDemoLogDecimal("worker_create_failed", 0U);
  // } else {
  //   /* 放开亲和性：允许在所有可用核上运行，适配 -bios none 下只有 hart0
  //    * 实际运行的情况。 */
  //   UBaseType_t uxMask =
  //       ((UBaseType_t)1U << (UBaseType_t)configNUMBER_OF_CORES) -
  //       (UBaseType_t)1U;
  //   vTaskCoreAffinitySet(xWorkerHandle, uxMask);
  //   if (xHeartbeatHandle != NULL) {
  //     vTaskCoreAffinitySet(xHeartbeatHandle, uxMask);
  //   }
  // }

  /* 在这里启动 DAG 运行时：初始化 DAG + 创建 DAG worker 任务 */
  vStartDagDemo();

  /* 固定亲和性：心跳跑在核0，worker 跑在核1，便于观察 SMP 行为。 */
  if (xHeartbeatHandle != NULL) {
    vTaskCoreAffinitySet(xHeartbeatHandle, (1U << 0));
  }
  // if (xWorkerHandle != NULL) {
  //   vTaskCoreAffinitySet(xWorkerHandle, (1U << 1));
  // }
}

/*-----------------------------------------------------------*/

// static void prvHeartbeatTask(void* pvParameters) {
//   (void)pvParameters;

//   // uint32_t ulPulse = 0U;
//   static int sReleasedSecondaries = 0;
//   // TickType_t xNext = xTaskGetTickCount();
//   // const TickType_t xPeriod = pdMS_TO_TICKS(1000U);
//   const TickType_t xDelay = 1000 / portTICK_PERIOD_MS;
//   int cnt = 0;
//   for (;;) {
//     // vDemoLogDecimal("h", ulPulse++);

//     if (sReleasedSecondaries == 0) {
//       vDemoLogString("[heartbeat] releasing secondaries\n");
//       prvWakeSecondaryHarts();
//       __atomic_store_n(&ulSecondaryRelease, 1U, __ATOMIC_RELEASE);
//       __asm volatile("fence w, rw" ::: "memory");
//       sReleasedSecondaries = 1;
//       vDemoLogString("[heartbeat] secondaries released\n");
//     }

//     vDemoLogDecimal("beat:", cnt++);
//     // vDemoLogString("[heartbeat] send\n");
//     // /* 每 1000ms 往 worker（核1）发送一个“递增序号” */
//     // if (xWorkerHandle != NULL) {
//     //   // xTaskNotifyGive(xWorkerHandle);
//     //   /* eSetValueWithOverwrite：若上次没来得及取，覆盖旧值，保持最新 */

//     //   /* 发送到通道 */
//     //   if (xChan != NULL) {
//     //     /* 发送给 worker 的序号 */
//     //     static uint32_t ulTxSeq = 0U;
//     //     (void)xQueueSend(xChan, &ulTxSeq, portMAX_DELAY);
//     //     ulTxSeq++;
//     //   }
//     // }

//     // vTaskDelayUntil(&xNext, xPeriod);
//     vTaskDelay(xDelay);
//   }
// }

static void prvHeartbeatTask(void* pvParameters) {
  (void)pvParameters;

  /* 只跑一次：唤醒所有 secondary 核 */
  vDemoLogString("[heartbeat] releasing secondaries\n");

  prvWakeSecondaryHarts();
  __atomic_store_n(&ulSecondaryRelease, 1U, __ATOMIC_RELEASE);
  __asm volatile("fence w, rw" ::: "memory");

  vDemoLogString("[heartbeat] secondaries released\n");

  /* 做一点点防御性延时，保证日志打出去（可选） */
  // vTaskDelay(pdMS_TO_TICKS(1));

  vDemoLogString("[heartbeat] exit\n");

  /* 自杀，释放 core0 调度给真正的业务任务 */
  vTaskDelete(NULL);

  /* 永远不会走到这里 */
  for (;;) {
  }
}

/*-----------------------------------------------------------*/

static void prvWorkerTask(void* pvParameters) {
  (void)pvParameters;

  // uint32_t ulIteration = 0U;

  uint32_t ulExpected = 0U, ulRxValue;

  for (;;) {
    // (void)ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    // uint32_t ulChecksum = prvBusyChecksum(ulIteration);
    // vDemoLogHexPair("worker", ulIteration, ulChecksum);
    // ulIteration++;

    /* 从心跳任务接收递增序号，检测是否有丢包或乱序 */
    if (xChan != NULL) {
      (void)xQueueReceive(xChan, &ulRxValue, portMAX_DELAY);
      if (ulRxValue == ulExpected) {
        vDemoLogDecimal("rx_ok", ulRxValue);
        ulExpected++;
      } else {
        // 可选：调试打印
        vDemoLogHexPair("rx_mismatch", ulExpected, ulRxValue);
        ulExpected = ulRxValue + 1U;
      }
    }
  }
}

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

// static uint32_t prvBusyChecksum(uint32_t ulSeed) {
//   uint32_t ulValue = ulSeed ^ 0xA5A5A5A5UL;

//   for (uint32_t i = 0U; i < 60000U; i++) {
//     ulValue ^= ((i + 1U) * 2654435761UL);
//     ulValue = (ulValue << 7) | (ulValue >> 25);
//   }

//   return ulValue;
// }

/*-----------------------------------------------------------*/

static void prvUartInit(void) {
  const uint16_t usDivisor = (uint16_t)(UART_CLOCK_HZ / (UART_BAUD * 16UL));

  *(volatile uint8_t*)(UART0_BASE + UART_REG_LCR) = UART_LCR_DLAB;
  *(volatile uint8_t*)(UART0_BASE + UART_REG_DLL) =
      (uint8_t)(usDivisor & 0xFFU);
  *(volatile uint8_t*)(UART0_BASE + UART_REG_DLM) = (uint8_t)(usDivisor >> 8);
  *(volatile uint8_t*)(UART0_BASE + UART_REG_LCR) = UART_LCR_8N1;
  *(volatile uint8_t*)(UART0_BASE + UART_REG_FCR) =
      UART_FCR_ENABLE | UART_FCR_CLEAR_RX | UART_FCR_CLEAR_TX;
  *(volatile uint8_t*)(UART0_BASE + UART_REG_MCR) = 0x00U;
}

/*-----------------------------------------------------------*/

static void prvUartPutRaw(char c) {
  volatile uint8_t* const pLSR = (volatile uint8_t*)(UART0_BASE + UART_REG_LSR);
  volatile uint8_t* const pTHR = (volatile uint8_t*)(UART0_BASE + UART_REG_THR);
  uint32_t spins = 0;
  /* Bounded wait to avoid pathological stalls if UART status misbehaves. */
  while (((*pLSR) & UART_LSR_TX_EMPTY) == 0U) {
    if (++spins > 1000000U) {
      break;
    }
  }
  *pTHR = (uint8_t)c;
}

static void prvUartWrite(const char* pcData, size_t xLength) {
  /* 阻塞式输出，保证关键日志可见。 */
  for (size_t i = 0U; i < xLength; i++) {
    char c = pcData[i];
    if (c == '\n') {
      prvUartPutRaw('\r');
    }
    prvUartPutRaw(c);
  }
}

static void vDemoLogString(const char* pcMsg) {
  prvUartWrite(pcMsg, strlen(pcMsg));
}

/*-----------------------------------------------------------*/

static void vDemoLogDecimal(const char* pcLabel, uint32_t ulValue) {
  char cBuffer[96];
  size_t xIndex = 0U;

  cBuffer[xIndex++] = '[';
  xIndex += prvWriteString(&cBuffer[xIndex], "core ");
  xIndex += prvWriteDecimal(&cBuffer[xIndex], (uint32_t)portGET_CORE_ID());
  cBuffer[xIndex++] = ']';
  cBuffer[xIndex++] = ' ';
  xIndex += prvWriteString(&cBuffer[xIndex], pcLabel);
  cBuffer[xIndex++] = ' ';
  xIndex += prvWriteDecimal(&cBuffer[xIndex], ulValue);
  cBuffer[xIndex++] = '\n';

  prvUartWrite(cBuffer, xIndex);
}

static void vDemoLogHexPair(const char* pcLabel, uint32_t ulFirst,
                            uint32_t ulSecond) {
  char cBuffer[128];
  size_t xIndex = 0U;

  cBuffer[xIndex++] = '[';
  xIndex += prvWriteString(&cBuffer[xIndex], "core ");
  xIndex += prvWriteDecimal(&cBuffer[xIndex], (uint32_t)portGET_CORE_ID());
  cBuffer[xIndex++] = ']';
  cBuffer[xIndex++] = ' ';
  xIndex += prvWriteString(&cBuffer[xIndex], pcLabel);
  cBuffer[xIndex++] = ' ';
  xIndex += prvWriteDecimal(&cBuffer[xIndex], ulFirst);
  xIndex += prvWriteString(&cBuffer[xIndex], " checksum=0x");
  xIndex += prvWriteHex(&cBuffer[xIndex], ulSecond);
  cBuffer[xIndex++] = '\n';

  prvUartWrite(cBuffer, xIndex);
}

/*-----------------------------------------------------------*/

static size_t prvWriteString(char* pcDest, const char* pcSrc) {
  size_t xLength = 0U;

  while (pcSrc[xLength] != '\0') {
    pcDest[xLength] = pcSrc[xLength];
    xLength++;
  }

  return xLength;
}

static size_t prvWriteDecimal(char* pcDest, uint32_t ulValue) {
  char cTemp[10];
  size_t xCount = 0U;

  do {
    cTemp[xCount++] = (char)('0' + (ulValue % 10UL));
    ulValue /= 10UL;
  } while (ulValue != 0UL);

  for (size_t i = 0U; i < xCount; i++) {
    pcDest[i] = cTemp[xCount - 1U - i];
  }

  return xCount;
}

static size_t prvWriteHex(char* pcDest, uint32_t ulValue) {
  static const char pcHex[] = "0123456789ABCDEF";

  for (size_t i = 0U; i < 8U; i++) {
    uint32_t shift = 28U - (uint32_t)(i * 4U);
    pcDest[i] = pcHex[(ulValue >> shift) & 0x0FU];
  }

  return 8U;
}

/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook(void) {
  taskDISABLE_INTERRUPTS();
  vDemoLogDecimal("malloc_failed", STACK_CHECK_VALUE);
  for (;;) {
  }
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName) {
  (void)xTask;
  vDemoLogString(pcTaskName);
  vDemoLogDecimal("stack_overflow", STACK_CHECK_VALUE);
  taskDISABLE_INTERRUPTS();
  for (;;) {
  }
}

void vAssertCalled(const char* pcFile, int lLine) {
  (void)pcFile;
  vDemoLogString(pcFile);
  vDemoLogDecimal("assert", (uint32_t)lLine);
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
/*-----------------------------------------------------------*/
