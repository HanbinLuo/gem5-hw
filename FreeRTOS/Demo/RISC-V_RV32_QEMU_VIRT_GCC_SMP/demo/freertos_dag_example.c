/*
 * freertos_dag_example.c
 *
 * DAG 示例（A -> (B, C) -> D），所有节点均异步卸载到 CU 执行。
 * 代码演示：
 *  - DAG runtime 拆分到 dag_runtime.[ch]
 *  - CU/DMA/PLIC 相关拆到 cu_offload.[ch] + plic_handler.h
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "FreeRTOS.h"
#include "cu_offload.h"
#include "dag_runtime.h"
#include "portmacro.h"
#include "task.h"
#include "uart16550.h"

#define NUM_WORKERS (configNUMBER_OF_CORES)
#define DAG_QUEUE_LEN 8
#define DAG_ASYNC_DONE_QUEUE_LEN 8

static DagNode nodeA, nodeB, nodeC, nodeD;
static DagData dataAtoB, dataAtoC, dataBtoD, dataCtoD;

/* ===================== 节点 run() 函数 ===================== */

typedef struct {
  uint32_t cu_id;
  uint32_t job_id;
  DagNode* node;
} CuNodeCtx_t;

/* nodeA 的 CU 上下文：随机选择空闲 CU，job_id = 0 */
static CuNodeCtx_t gCtxA;

/* nodeB 的 CU 上下文：随机选择空闲 CU，job_id = 1 */
static CuNodeCtx_t gCtxB;

/* nodeC 的 CU 上下文：随机选择空闲 CU，job_id = 2 */
static CuNodeCtx_t gCtxC;

/* nodeD 的 CU 上下文：随机选择空闲 CU，job_id = 3 */
static CuNodeCtx_t gCtxD;

static void vCuNodeWork(void* arg) {
  CuNodeCtx_t* ctx = (CuNodeCtx_t*)arg;

  LOGF("Node %s start on core %d\n", ctx->node->name,
       (uint32_t)portGET_CORE_ID());

  ctx->node->async_pending = 1U;
  uint32_t actual_cu = vCuSubmitDagJob(ctx->cu_id, ctx->job_id, ctx->node);
  LOGF("Node %s submitted to CU %d on core %d\n", ctx->node->name, actual_cu,
       (uint32_t)portGET_CORE_ID());
}

/* ===================== DAG 初始化 ===================== */

static void vDagInit(void) {
  static DagNode* succOfA[2];
  static DagNode* succOfB[1];
  static DagNode* succOfC[1];
  static DagData* inputsB[1];
  static DagData* inputsC[1];
  static DagData* inputsD[2];
  static DagData* outputsA[2];
  static DagData* outputsB[1];
  static DagData* outputsC[1];

  memset(&nodeA, 0, sizeof(nodeA));
  memset(&nodeB, 0, sizeof(nodeB));
  memset(&nodeC, 0, sizeof(nodeC));
  memset(&nodeD, 0, sizeof(nodeD));
  memset(&dataAtoB, 0, sizeof(dataAtoB));
  memset(&dataAtoC, 0, sizeof(dataAtoC));
  memset(&dataBtoD, 0, sizeof(dataBtoD));
  memset(&dataCtoD, 0, sizeof(dataCtoD));

  dataAtoB.name = "A->B";
  dataAtoB.sizeBytes = 64U;
  dataAtoB.consumers = 1U;

  dataAtoC.name = "A->C";
  dataAtoC.sizeBytes = 32U;
  dataAtoC.consumers = 1U;

  dataBtoD.name = "B->D";
  dataBtoD.sizeBytes = 48U;
  dataBtoD.consumers = 1U;

  dataCtoD.name = "C->D";
  dataCtoD.sizeBytes = 16U;
  dataCtoD.consumers = 1U;

  nodeA.name = "A";
  nodeA.run = vCuNodeWork;
  nodeA.arg = (void*)&gCtxA;
  nodeA.inputSizeBytes = 0U;
  nodeA.outputSizeBytes = dataAtoB.sizeBytes + dataAtoC.sizeBytes;
  nodeA.computeDelayMs = UINT32_MAX;
  nodeA.storageSizeBytes = 128U;
  nodeA.numInputs = 0U;
  nodeA.inputs = NULL;
  nodeA.numOutputs = 2U;
  nodeA.outputs = outputsA;
  nodeA.indegree = 0U;
  nodeA.numSuccessors = 2U;
  nodeA.successors = succOfA;
  nodeA.mode = DAG_NODE_ASYNC;

  /* 配置 nodeA 提交到任意空闲 CU, job_id=0 */
  gCtxA.cu_id = CU_ANY_ID;
  gCtxA.job_id = 0U;
  gCtxA.node = &nodeA;

  /* 配置 nodeB 提交到任意空闲 CU, job_id=1 */
  gCtxB.cu_id = CU_ANY_ID;
  gCtxB.job_id = 1U;
  gCtxB.node = &nodeB;

  /* 配置 nodeC 提交到任意空闲 CU, job_id=2 */
  gCtxC.cu_id = CU_ANY_ID;
  gCtxC.job_id = 2U;
  gCtxC.node = &nodeC;

  nodeB.name = "B";
  nodeB.run = vCuNodeWork;
  nodeB.arg = (void*)&gCtxB;
  nodeB.inputSizeBytes = dataAtoB.sizeBytes;
  nodeB.outputSizeBytes = dataBtoD.sizeBytes;
  nodeB.computeDelayMs = UINT32_MAX;
  nodeB.storageSizeBytes = 64U;
  nodeB.numInputs = 1U;
  nodeB.inputs = inputsB;
  nodeB.numOutputs = 1U;
  nodeB.outputs = outputsB;
  nodeB.indegree = 1U;
  nodeB.numSuccessors = 1U;
  nodeB.successors = succOfB;
  nodeB.mode = DAG_NODE_ASYNC;

  /* nodeC 行为与 nodeB 类似：
   *  - 由运行时选择空闲 CU 提交
   *  - 在 run() 中设置 node->async_pending = 1 并调用 vCuSubmitDagJob()
   *  - CU 完成后由 PLIC 中断触发 vCuHandleIsr(...)
   *  - ISR 通过 vDagNotifyAsyncDoneFromISR() 通知 DAG runtime
   *  - 由 async 任务处理后触发后继节点 D */
  nodeC.name = "C";
  nodeC.run = vCuNodeWork; /* 异步提交到空闲 CU */
  nodeC.arg = (void*)&gCtxC;
  nodeC.inputSizeBytes = dataAtoC.sizeBytes;
  nodeC.outputSizeBytes = dataCtoD.sizeBytes;
  nodeC.computeDelayMs = UINT32_MAX;
  nodeC.storageSizeBytes = 64U;
  nodeC.numInputs = 1U;
  nodeC.inputs = inputsC;
  nodeC.numOutputs = 1U;
  nodeC.outputs = outputsC;
  nodeC.indegree = 1U;
  nodeC.numSuccessors = 1U;
  nodeC.successors = succOfC;
  nodeC.mode = DAG_NODE_ASYNC;

  nodeD.name = "D";
  nodeD.run = vCuNodeWork;
  nodeD.arg = (void*)&gCtxD;
  nodeD.inputSizeBytes = dataBtoD.sizeBytes + dataCtoD.sizeBytes;
  nodeD.outputSizeBytes = 0U;
  nodeD.computeDelayMs = UINT32_MAX;
  nodeD.storageSizeBytes = 256U;
  nodeD.numInputs = 2U;
  nodeD.inputs = inputsD;
  nodeD.numOutputs = 0U;
  nodeD.outputs = NULL;
  nodeD.indegree = 2U;
  nodeD.numSuccessors = 0U;
  nodeD.successors = NULL;
  nodeD.mode = DAG_NODE_ASYNC;

  /* 配置 nodeD 提交到任意空闲 CU, job_id=3 */
  gCtxD.cu_id = CU_ANY_ID;
  gCtxD.job_id = 3U;
  gCtxD.node = &nodeD;

  outputsA[0] = &dataAtoB;
  outputsA[1] = &dataAtoC;
  outputsB[0] = &dataBtoD;
  outputsC[0] = &dataCtoD;

  inputsB[0] = &dataAtoB;
  inputsC[0] = &dataAtoC;
  inputsD[0] = &dataBtoD;
  inputsD[1] = &dataCtoD;

  succOfA[0] = &nodeB;
  succOfA[1] = &nodeC;
  succOfB[0] = &nodeD;
  succOfC[0] = &nodeD;
}

/* ===================== 对外启动接口 ===================== */

void vStartDagDemo(void) {
  vDagInit();
  vCuInit();

  DagRuntimeConfig cfg = {.workerCount = NUM_WORKERS,
                          .workerPriority = tskIDLE_PRIORITY + 2,
                          .workerStackWords = 512,
                          .asyncPriority = tskIDLE_PRIORITY + 3,
                          .asyncStackWords = 512,
                          .readyQueueLen = DAG_QUEUE_LEN,
                          .asyncDoneQueueLen = DAG_ASYNC_DONE_QUEUE_LEN,
                          .useCoreAffinity = 1};

  vDagRuntimeStart(&cfg);

  DagNode* root = &nodeA;
  vDagSubmitReadyNode(root);
}

/*-----------------------------------------------------------*/
