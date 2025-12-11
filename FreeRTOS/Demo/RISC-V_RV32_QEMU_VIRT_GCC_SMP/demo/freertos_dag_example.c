/*
 * freertos_dag_example.c
 *
 * DAG 示例（A -> (B, C) -> D），其中 B 为异步卸载到 CU 的节点。
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

/* ===================== 节点 run() 函数 ===================== */

static void vDummyWork(void* arg) {
  const char* name = (const char*)arg;

  LOGF("Node %s start on core %d\n", name, (uint32_t)portGET_CORE_ID());

  vTaskDelay(pdMS_TO_TICKS(10));

  LOGF("Node %s done on core %d\n", name, (uint32_t)portGET_CORE_ID());
}

typedef struct {
  uint32_t cu_id;
  uint32_t job_id;
  DagNode* node;
} CuNodeCtx_t;

static CuNodeCtx_t gCtxB;

static void vCuNodeWork(void* arg) {
  CuNodeCtx_t* ctx = (CuNodeCtx_t*)arg;

  LOGF("Node %s start on core %d\n", ctx->node->name,
       (uint32_t)portGET_CORE_ID());

  ctx->node->async_pending = 1U;
  vCuSubmitDagJob(ctx->cu_id, ctx->job_id, ctx->node);
  LOGF("Node %s submitted to CU %d on core %d\n", ctx->node->name, ctx->cu_id,
       (uint32_t)portGET_CORE_ID());
}

/* ===================== DAG 初始化 ===================== */

static void vDagInit(void) {
  static DagNode* succOfA[2];
  static DagNode* succOfB[1];
  static DagNode* succOfC[1];

  memset(&nodeA, 0, sizeof(nodeA));
  memset(&nodeB, 0, sizeof(nodeB));
  memset(&nodeC, 0, sizeof(nodeC));
  memset(&nodeD, 0, sizeof(nodeD));

  nodeA.name = "A";
  nodeA.run = vDummyWork;
  nodeA.arg = (void*)"A";
  nodeA.indegree = 0U;
  nodeA.numSuccessors = 2U;
  nodeA.successors = succOfA;
  nodeA.mode = DAG_NODE_SYNC;

  gCtxB.cu_id = 0U;
  gCtxB.job_id = 1U;
  gCtxB.node = &nodeB;

  nodeB.name = "B";
  nodeB.run = vCuNodeWork;
  nodeB.arg = (void*)&gCtxB;
  nodeB.indegree = 1U;
  nodeB.numSuccessors = 1U;
  nodeB.successors = succOfB;
  nodeB.mode = DAG_NODE_ASYNC;

  nodeC.name = "C";
  nodeC.run = vDummyWork;
  nodeC.arg = (void*)"C";
  nodeC.indegree = 1U;
  nodeC.numSuccessors = 1U;
  nodeC.successors = succOfC;
  nodeC.mode = DAG_NODE_SYNC;

  nodeD.name = "D";
  nodeD.run = vDummyWork;
  nodeD.arg = (void*)"D";
  nodeD.indegree = 2U;
  nodeD.numSuccessors = 0U;
  nodeD.successors = NULL;
  nodeD.mode = DAG_NODE_SYNC;

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
