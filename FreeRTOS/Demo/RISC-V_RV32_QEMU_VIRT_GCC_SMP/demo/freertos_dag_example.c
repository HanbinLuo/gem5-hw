/*
 * freertos_dag_example.c
 *
 * 简单的 DAG 调度示例：
 * A -> (B, C 并行) -> D
 *
 * - 使用 FreeRTOS SMP，多个 worker 任务从队列中取 DAG 节点执行
 * - 节点之间依赖关系用入度（indegree）+ 后继列表表示
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

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

/* ===================== DAG 数据结构 ===================== */

typedef struct DagNode {
  const char* name;
  void (*run)(void* arg);
  void* arg;

  uint32_t indegree;
  uint32_t numSuccessors;
  struct DagNode** successors;
} DagNode;

/* ===================== 全局对象声明 ===================== */

// #define NUM_WORKERS (configNUMBER_OF_CORES - 1)
#define NUM_WORKERS configNUMBER_OF_CORES
#define DAG_QUEUE_LEN 8

static DagNode nodeA, nodeB, nodeC, nodeD;
static QueueHandle_t xDagReadyQ;

/* ===================== 辅助函数 ===================== */

static inline uint32_t dag_atomic_dec(uint32_t* pValue) {
  uint32_t newVal;

  taskENTER_CRITICAL();
  *pValue = *pValue - 1;
  newVal = *pValue;
  taskEXIT_CRITICAL();

  return newVal;
}

/* 简单的节点工作函数 */
static void vDummyWork(void* arg) {
  const char* name = (const char*)arg;

  vDemoLogString(name);
  vDemoLogDecimal("Node start on core", (uint32_t)portGET_CORE_ID());
  //   printf("Node %s start on core %ld\r\n", name, (long)portGET_CORE_ID());

  vTaskDelay(pdMS_TO_TICKS(10));

  vDemoLogString(name);
  vDemoLogDecimal("Node done on core", (uint32_t)portGET_CORE_ID());
  //   printf("Node %s done on core %ld\r\n", name, (long)portGET_CORE_ID());
}

/* ===================== DAG 初始化 ===================== */

static void vDagInit(void) {
  static DagNode* succOfA[2];
  static DagNode* succOfB[1];
  static DagNode* succOfC[1];

  nodeA.name = "A";
  nodeA.run = vDummyWork;
  nodeA.arg = (void*)"A";
  nodeA.indegree = 0;
  nodeA.numSuccessors = 2;
  nodeA.successors = succOfA;

  nodeB.name = "B";
  nodeB.run = vDummyWork;
  nodeB.arg = (void*)"B";
  nodeB.indegree = 1;
  nodeB.numSuccessors = 1;
  nodeB.successors = succOfB;

  nodeC.name = "C";
  nodeC.run = vDummyWork;
  nodeC.arg = (void*)"C";
  nodeC.indegree = 1;
  nodeC.numSuccessors = 1;
  nodeC.successors = succOfC;

  nodeD.name = "D";
  nodeD.run = vDummyWork;
  nodeD.arg = (void*)"D";
  nodeD.indegree = 2;
  nodeD.numSuccessors = 0;
  nodeD.successors = NULL;

  succOfA[0] = &nodeB;
  succOfA[1] = &nodeC;

  succOfB[0] = &nodeD;
  succOfC[0] = &nodeD;

  xDagReadyQ = xQueueCreate(DAG_QUEUE_LEN, sizeof(DagNode*));
  configASSERT(xDagReadyQ != NULL);

  DagNode* pNode = &nodeA;
  xQueueSend(xDagReadyQ, &pNode, portMAX_DELAY);
}

/* ===================== Worker 任务 ===================== */

static void vDagWorkerTask(void* pvParam) {
  // (void)pvParam;
  DagNode* node;

  UBaseType_t freeWords = uxTaskGetStackHighWaterMark(NULL);
  vDemoLogDecimal("worker_free_stack_words", (uint32_t)freeWords);
  vTaskDelay(pdMS_TO_TICKS(1));
  // TickType_t xNext = xTaskGetTickCount();
  // const TickType_t xPeriod = pdMS_TO_TICKS(1000U);
  // const TickType_t xDelay = 1000 / portTICK_PERIOD_MS;
  // int cnt = 0;

  for (;;) {
    // vTaskDelay(xDelay);
    // vDemoLogDecimal("worker_free_stack_words", (uint32_t)freeWords);
    // vDemoLogString((const char*)pvParam);
    // vDemoLogDecimal("test:", cnt++);
    if (xQueueReceive(xDagReadyQ, &node, portMAX_DELAY) == pdTRUE) {
      node->run(node->arg);

      for (uint32_t i = 0; i < node->numSuccessors; i++) {
        DagNode* succ = node->successors[i];
        uint32_t newIndegree = dag_atomic_dec(&succ->indegree);

        if (newIndegree == 0) {
          DagNode* ready = succ;
          xQueueSend(xDagReadyQ, &ready, portMAX_DELAY);
        }
      }
    }
    // vTaskDelayUntil(&xNext, xPeriod);
  }
}

static TaskHandle_t xDagWorkers[NUM_WORKERS];
static char workerName[NUM_WORKERS][32] = {"DagWorker0\n", "DagWorker1\n",
                                           "DagWorker2\n"};

/* ===================== 入口示例 ===================== */

void vStartDagDemo(void) {
  vDagInit();

  vDemoLogDecimal("NUM_WORKERS", NUM_WORKERS);

  for (int i = 0; i < NUM_WORKERS; i++) {
    // char name[16];
    // snprintf(name, sizeof(name), "DagWorker%d", i);

    BaseType_t rc =
        xTaskCreate(vDagWorkerTask, workerName[i], 512, workerName[i],
                    tskIDLE_PRIORITY + 2, &xDagWorkers[i]);

    configASSERT(rc == pdPASS);

#if (configUSE_CORE_AFFINITY == 1)
    // vTaskCoreAffinitySet(xDagWorkers[i], (1U << (i + 1)));
    vTaskCoreAffinitySet(xDagWorkers[i], (1U << i));
#endif
  }
}

/*
 * 在你的 main() 或系统初始化代码中调用：
 *
 *   vStartDagDemo();
 *
 * 然后启动调度器：
 *
 *   vTaskStartScheduler();
 *
 * 实际项目里你可以把各个节点的 run 函数替换为真正的计算/处理逻辑，
 * 并将 DAG 节点扩展为更大的图结构。
 */

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