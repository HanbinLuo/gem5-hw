/* Auto-generated DAG from dag1.json */
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
#define DAG_QUEUE_LEN 16
#define DAG_ASYNC_DONE_QUEUE_LEN 16

typedef struct { uint32_t cu_id; uint32_t job_id; DagNode* node; } CuNodeCtx_t;

static DagNode node_Task_ifft_0;
static DagNode node_Task_ifft_1_1;
static DagNode node_Task_ifft_2_2;
static DagNode node_Task_ifft_3_3;
static DagNode node_Task_ifft_4_4;
static DagNode node_Task_ifft_5_5;
static DagNode node_Task_fft_6;
static DagNode node_Task_fft_1_7;
static DagNode node_Task_fft_2_8;
static DagNode node_Task_fft_3_9;
static DagNode node_Task_fft_4_10;
static DagNode node_Task_fft_5_11;

static DagData data_Task_ifft_0_out0;
static DagData data_Task_ifft_0_out1;
static DagData data_Task_ifft_1_1_out0;
static DagData data_Task_ifft_1_1_out1;
static DagData data_Task_ifft_2_2_out0;
static DagData data_Task_ifft_2_2_out1;
static DagData data_Task_ifft_3_3_out0;
static DagData data_Task_ifft_3_3_out1;
static DagData data_Task_ifft_4_4_out0;
static DagData data_Task_ifft_4_4_out1;
static DagData data_Task_ifft_5_5_out0;
static DagData data_Task_ifft_5_5_out1;

static CuNodeCtx_t ctx_Task_ifft_0;
static CuNodeCtx_t ctx_Task_ifft_1_1;
static CuNodeCtx_t ctx_Task_ifft_2_2;
static CuNodeCtx_t ctx_Task_ifft_3_3;
static CuNodeCtx_t ctx_Task_ifft_4_4;
static CuNodeCtx_t ctx_Task_ifft_5_5;
static CuNodeCtx_t ctx_Task_fft_6;
static CuNodeCtx_t ctx_Task_fft_1_7;
static CuNodeCtx_t ctx_Task_fft_2_8;
static CuNodeCtx_t ctx_Task_fft_3_9;
static CuNodeCtx_t ctx_Task_fft_4_10;
static CuNodeCtx_t ctx_Task_fft_5_11;

static DagNode* succOf_Task_ifft_0[1];
static DagData* outputs_Task_ifft_0[2];
static DagNode* succOf_Task_ifft_1_1[1];
static DagData* outputs_Task_ifft_1_1[2];
static DagNode* succOf_Task_ifft_2_2[1];
static DagData* outputs_Task_ifft_2_2[2];
static DagNode* succOf_Task_ifft_3_3[1];
static DagData* outputs_Task_ifft_3_3[2];
static DagNode* succOf_Task_ifft_4_4[1];
static DagData* outputs_Task_ifft_4_4[2];
static DagNode* succOf_Task_ifft_5_5[1];
static DagData* outputs_Task_ifft_5_5[2];
static DagData* inputs_Task_fft_6[2];
static DagData* inputs_Task_fft_1_7[2];
static DagData* inputs_Task_fft_2_8[2];
static DagData* inputs_Task_fft_3_9[2];
static DagData* inputs_Task_fft_4_10[2];
static DagData* inputs_Task_fft_5_11[2];

static void vCuNodeWork(void* arg) {
  CuNodeCtx_t* ctx = (CuNodeCtx_t*)arg;
  ctx->node->async_pending = 1U;
  vCuSubmitDagJob(ctx->cu_id, ctx->job_id, ctx->node);
}

static void vDagInit(void) {
  memset(&data_Task_ifft_0_out0, 0, sizeof(DagData));
  data_Task_ifft_0_out0.name = "Task_ifft_0_out0";
  data_Task_ifft_0_out0.sizeBytes = 64U;
  data_Task_ifft_0_out0.consumers = 1U;
  memset(&data_Task_ifft_0_out1, 0, sizeof(DagData));
  data_Task_ifft_0_out1.name = "Task_ifft_0_out1";
  data_Task_ifft_0_out1.sizeBytes = 64U;
  data_Task_ifft_0_out1.consumers = 1U;
  memset(&data_Task_ifft_1_1_out0, 0, sizeof(DagData));
  data_Task_ifft_1_1_out0.name = "Task_ifft_1_1_out0";
  data_Task_ifft_1_1_out0.sizeBytes = 64U;
  data_Task_ifft_1_1_out0.consumers = 1U;
  memset(&data_Task_ifft_1_1_out1, 0, sizeof(DagData));
  data_Task_ifft_1_1_out1.name = "Task_ifft_1_1_out1";
  data_Task_ifft_1_1_out1.sizeBytes = 64U;
  data_Task_ifft_1_1_out1.consumers = 1U;
  memset(&data_Task_ifft_2_2_out0, 0, sizeof(DagData));
  data_Task_ifft_2_2_out0.name = "Task_ifft_2_2_out0";
  data_Task_ifft_2_2_out0.sizeBytes = 64U;
  data_Task_ifft_2_2_out0.consumers = 1U;
  memset(&data_Task_ifft_2_2_out1, 0, sizeof(DagData));
  data_Task_ifft_2_2_out1.name = "Task_ifft_2_2_out1";
  data_Task_ifft_2_2_out1.sizeBytes = 64U;
  data_Task_ifft_2_2_out1.consumers = 1U;
  memset(&data_Task_ifft_3_3_out0, 0, sizeof(DagData));
  data_Task_ifft_3_3_out0.name = "Task_ifft_3_3_out0";
  data_Task_ifft_3_3_out0.sizeBytes = 64U;
  data_Task_ifft_3_3_out0.consumers = 1U;
  memset(&data_Task_ifft_3_3_out1, 0, sizeof(DagData));
  data_Task_ifft_3_3_out1.name = "Task_ifft_3_3_out1";
  data_Task_ifft_3_3_out1.sizeBytes = 64U;
  data_Task_ifft_3_3_out1.consumers = 1U;
  memset(&data_Task_ifft_4_4_out0, 0, sizeof(DagData));
  data_Task_ifft_4_4_out0.name = "Task_ifft_4_4_out0";
  data_Task_ifft_4_4_out0.sizeBytes = 64U;
  data_Task_ifft_4_4_out0.consumers = 1U;
  memset(&data_Task_ifft_4_4_out1, 0, sizeof(DagData));
  data_Task_ifft_4_4_out1.name = "Task_ifft_4_4_out1";
  data_Task_ifft_4_4_out1.sizeBytes = 64U;
  data_Task_ifft_4_4_out1.consumers = 1U;
  memset(&data_Task_ifft_5_5_out0, 0, sizeof(DagData));
  data_Task_ifft_5_5_out0.name = "Task_ifft_5_5_out0";
  data_Task_ifft_5_5_out0.sizeBytes = 64U;
  data_Task_ifft_5_5_out0.consumers = 1U;
  memset(&data_Task_ifft_5_5_out1, 0, sizeof(DagData));
  data_Task_ifft_5_5_out1.name = "Task_ifft_5_5_out1";
  data_Task_ifft_5_5_out1.sizeBytes = 64U;
  data_Task_ifft_5_5_out1.consumers = 1U;

  /* Node Task_ifft */
  memset(&node_Task_ifft_0, 0, sizeof(DagNode));
  node_Task_ifft_0.name = "Task_ifft";
  node_Task_ifft_0.run = vCuNodeWork;
  node_Task_ifft_0.arg = &ctx_Task_ifft_0;
  node_Task_ifft_0.storageSizeBytes = 2048U;
  node_Task_ifft_0.indegree = 0U;
  node_Task_ifft_0.mode = DAG_NODE_ASYNC;
  node_Task_ifft_0.numOutputs = 2U;
  node_Task_ifft_0.outputs = outputs_Task_ifft_0;
  outputs_Task_ifft_0[0] = &data_Task_ifft_0_out0;
  outputs_Task_ifft_0[1] = &data_Task_ifft_0_out1;
  node_Task_ifft_0.numSuccessors = 1U;
  node_Task_ifft_0.successors = succOf_Task_ifft_0;
  succOf_Task_ifft_0[0] = &node_Task_fft_6;
  ctx_Task_ifft_0.cu_id = CU_ANY_ID;
  ctx_Task_ifft_0.job_id = 0U;
  ctx_Task_ifft_0.node = &node_Task_ifft_0;

  /* Node Task_ifft*1 */
  memset(&node_Task_ifft_1_1, 0, sizeof(DagNode));
  node_Task_ifft_1_1.name = "Task_ifft*1";
  node_Task_ifft_1_1.run = vCuNodeWork;
  node_Task_ifft_1_1.arg = &ctx_Task_ifft_1_1;
  node_Task_ifft_1_1.storageSizeBytes = 2048U;
  node_Task_ifft_1_1.indegree = 0U;
  node_Task_ifft_1_1.mode = DAG_NODE_ASYNC;
  node_Task_ifft_1_1.numOutputs = 2U;
  node_Task_ifft_1_1.outputs = outputs_Task_ifft_1_1;
  outputs_Task_ifft_1_1[0] = &data_Task_ifft_1_1_out0;
  outputs_Task_ifft_1_1[1] = &data_Task_ifft_1_1_out1;
  node_Task_ifft_1_1.numSuccessors = 1U;
  node_Task_ifft_1_1.successors = succOf_Task_ifft_1_1;
  succOf_Task_ifft_1_1[0] = &node_Task_fft_1_7;
  ctx_Task_ifft_1_1.cu_id = CU_ANY_ID;
  ctx_Task_ifft_1_1.job_id = 1U;
  ctx_Task_ifft_1_1.node = &node_Task_ifft_1_1;

  /* Node Task_ifft*2 */
  memset(&node_Task_ifft_2_2, 0, sizeof(DagNode));
  node_Task_ifft_2_2.name = "Task_ifft*2";
  node_Task_ifft_2_2.run = vCuNodeWork;
  node_Task_ifft_2_2.arg = &ctx_Task_ifft_2_2;
  node_Task_ifft_2_2.storageSizeBytes = 2048U;
  node_Task_ifft_2_2.indegree = 0U;
  node_Task_ifft_2_2.mode = DAG_NODE_ASYNC;
  node_Task_ifft_2_2.numOutputs = 2U;
  node_Task_ifft_2_2.outputs = outputs_Task_ifft_2_2;
  outputs_Task_ifft_2_2[0] = &data_Task_ifft_2_2_out0;
  outputs_Task_ifft_2_2[1] = &data_Task_ifft_2_2_out1;
  node_Task_ifft_2_2.numSuccessors = 1U;
  node_Task_ifft_2_2.successors = succOf_Task_ifft_2_2;
  succOf_Task_ifft_2_2[0] = &node_Task_fft_2_8;
  ctx_Task_ifft_2_2.cu_id = CU_ANY_ID;
  ctx_Task_ifft_2_2.job_id = 2U;
  ctx_Task_ifft_2_2.node = &node_Task_ifft_2_2;

  /* Node Task_ifft*3 */
  memset(&node_Task_ifft_3_3, 0, sizeof(DagNode));
  node_Task_ifft_3_3.name = "Task_ifft*3";
  node_Task_ifft_3_3.run = vCuNodeWork;
  node_Task_ifft_3_3.arg = &ctx_Task_ifft_3_3;
  node_Task_ifft_3_3.storageSizeBytes = 2048U;
  node_Task_ifft_3_3.indegree = 0U;
  node_Task_ifft_3_3.mode = DAG_NODE_ASYNC;
  node_Task_ifft_3_3.numOutputs = 2U;
  node_Task_ifft_3_3.outputs = outputs_Task_ifft_3_3;
  outputs_Task_ifft_3_3[0] = &data_Task_ifft_3_3_out0;
  outputs_Task_ifft_3_3[1] = &data_Task_ifft_3_3_out1;
  node_Task_ifft_3_3.numSuccessors = 1U;
  node_Task_ifft_3_3.successors = succOf_Task_ifft_3_3;
  succOf_Task_ifft_3_3[0] = &node_Task_fft_3_9;
  ctx_Task_ifft_3_3.cu_id = CU_ANY_ID;
  ctx_Task_ifft_3_3.job_id = 3U;
  ctx_Task_ifft_3_3.node = &node_Task_ifft_3_3;

  /* Node Task_ifft*4 */
  memset(&node_Task_ifft_4_4, 0, sizeof(DagNode));
  node_Task_ifft_4_4.name = "Task_ifft*4";
  node_Task_ifft_4_4.run = vCuNodeWork;
  node_Task_ifft_4_4.arg = &ctx_Task_ifft_4_4;
  node_Task_ifft_4_4.storageSizeBytes = 2048U;
  node_Task_ifft_4_4.indegree = 0U;
  node_Task_ifft_4_4.mode = DAG_NODE_ASYNC;
  node_Task_ifft_4_4.numOutputs = 2U;
  node_Task_ifft_4_4.outputs = outputs_Task_ifft_4_4;
  outputs_Task_ifft_4_4[0] = &data_Task_ifft_4_4_out0;
  outputs_Task_ifft_4_4[1] = &data_Task_ifft_4_4_out1;
  node_Task_ifft_4_4.numSuccessors = 1U;
  node_Task_ifft_4_4.successors = succOf_Task_ifft_4_4;
  succOf_Task_ifft_4_4[0] = &node_Task_fft_4_10;
  ctx_Task_ifft_4_4.cu_id = CU_ANY_ID;
  ctx_Task_ifft_4_4.job_id = 4U;
  ctx_Task_ifft_4_4.node = &node_Task_ifft_4_4;

  /* Node Task_ifft*5 */
  memset(&node_Task_ifft_5_5, 0, sizeof(DagNode));
  node_Task_ifft_5_5.name = "Task_ifft*5";
  node_Task_ifft_5_5.run = vCuNodeWork;
  node_Task_ifft_5_5.arg = &ctx_Task_ifft_5_5;
  node_Task_ifft_5_5.storageSizeBytes = 2048U;
  node_Task_ifft_5_5.indegree = 0U;
  node_Task_ifft_5_5.mode = DAG_NODE_ASYNC;
  node_Task_ifft_5_5.numOutputs = 2U;
  node_Task_ifft_5_5.outputs = outputs_Task_ifft_5_5;
  outputs_Task_ifft_5_5[0] = &data_Task_ifft_5_5_out0;
  outputs_Task_ifft_5_5[1] = &data_Task_ifft_5_5_out1;
  node_Task_ifft_5_5.numSuccessors = 1U;
  node_Task_ifft_5_5.successors = succOf_Task_ifft_5_5;
  succOf_Task_ifft_5_5[0] = &node_Task_fft_5_11;
  ctx_Task_ifft_5_5.cu_id = CU_ANY_ID;
  ctx_Task_ifft_5_5.job_id = 5U;
  ctx_Task_ifft_5_5.node = &node_Task_ifft_5_5;

  /* Node Task_fft */
  memset(&node_Task_fft_6, 0, sizeof(DagNode));
  node_Task_fft_6.name = "Task_fft";
  node_Task_fft_6.run = vCuNodeWork;
  node_Task_fft_6.arg = &ctx_Task_fft_6;
  node_Task_fft_6.storageSizeBytes = 1920U;
  node_Task_fft_6.indegree = 2U;
  node_Task_fft_6.mode = DAG_NODE_ASYNC;
  node_Task_fft_6.numInputs = 2U;
  node_Task_fft_6.inputs = inputs_Task_fft_6;
  inputs_Task_fft_6[0] = &data_Task_ifft_0_out0;
  inputs_Task_fft_6[1] = &data_Task_ifft_0_out1;
  ctx_Task_fft_6.cu_id = CU_ANY_ID;
  ctx_Task_fft_6.job_id = 6U;
  ctx_Task_fft_6.node = &node_Task_fft_6;

  /* Node Task_fft*1 */
  memset(&node_Task_fft_1_7, 0, sizeof(DagNode));
  node_Task_fft_1_7.name = "Task_fft*1";
  node_Task_fft_1_7.run = vCuNodeWork;
  node_Task_fft_1_7.arg = &ctx_Task_fft_1_7;
  node_Task_fft_1_7.storageSizeBytes = 1920U;
  node_Task_fft_1_7.indegree = 2U;
  node_Task_fft_1_7.mode = DAG_NODE_ASYNC;
  node_Task_fft_1_7.numInputs = 2U;
  node_Task_fft_1_7.inputs = inputs_Task_fft_1_7;
  inputs_Task_fft_1_7[0] = &data_Task_ifft_1_1_out0;
  inputs_Task_fft_1_7[1] = &data_Task_ifft_1_1_out1;
  ctx_Task_fft_1_7.cu_id = CU_ANY_ID;
  ctx_Task_fft_1_7.job_id = 7U;
  ctx_Task_fft_1_7.node = &node_Task_fft_1_7;

  /* Node Task_fft*2 */
  memset(&node_Task_fft_2_8, 0, sizeof(DagNode));
  node_Task_fft_2_8.name = "Task_fft*2";
  node_Task_fft_2_8.run = vCuNodeWork;
  node_Task_fft_2_8.arg = &ctx_Task_fft_2_8;
  node_Task_fft_2_8.storageSizeBytes = 1920U;
  node_Task_fft_2_8.indegree = 2U;
  node_Task_fft_2_8.mode = DAG_NODE_ASYNC;
  node_Task_fft_2_8.numInputs = 2U;
  node_Task_fft_2_8.inputs = inputs_Task_fft_2_8;
  inputs_Task_fft_2_8[0] = &data_Task_ifft_2_2_out0;
  inputs_Task_fft_2_8[1] = &data_Task_ifft_2_2_out1;
  ctx_Task_fft_2_8.cu_id = CU_ANY_ID;
  ctx_Task_fft_2_8.job_id = 8U;
  ctx_Task_fft_2_8.node = &node_Task_fft_2_8;

  /* Node Task_fft*3 */
  memset(&node_Task_fft_3_9, 0, sizeof(DagNode));
  node_Task_fft_3_9.name = "Task_fft*3";
  node_Task_fft_3_9.run = vCuNodeWork;
  node_Task_fft_3_9.arg = &ctx_Task_fft_3_9;
  node_Task_fft_3_9.storageSizeBytes = 1920U;
  node_Task_fft_3_9.indegree = 2U;
  node_Task_fft_3_9.mode = DAG_NODE_ASYNC;
  node_Task_fft_3_9.numInputs = 2U;
  node_Task_fft_3_9.inputs = inputs_Task_fft_3_9;
  inputs_Task_fft_3_9[0] = &data_Task_ifft_3_3_out0;
  inputs_Task_fft_3_9[1] = &data_Task_ifft_3_3_out1;
  ctx_Task_fft_3_9.cu_id = CU_ANY_ID;
  ctx_Task_fft_3_9.job_id = 9U;
  ctx_Task_fft_3_9.node = &node_Task_fft_3_9;

  /* Node Task_fft*4 */
  memset(&node_Task_fft_4_10, 0, sizeof(DagNode));
  node_Task_fft_4_10.name = "Task_fft*4";
  node_Task_fft_4_10.run = vCuNodeWork;
  node_Task_fft_4_10.arg = &ctx_Task_fft_4_10;
  node_Task_fft_4_10.storageSizeBytes = 1920U;
  node_Task_fft_4_10.indegree = 2U;
  node_Task_fft_4_10.mode = DAG_NODE_ASYNC;
  node_Task_fft_4_10.numInputs = 2U;
  node_Task_fft_4_10.inputs = inputs_Task_fft_4_10;
  inputs_Task_fft_4_10[0] = &data_Task_ifft_4_4_out0;
  inputs_Task_fft_4_10[1] = &data_Task_ifft_4_4_out1;
  ctx_Task_fft_4_10.cu_id = CU_ANY_ID;
  ctx_Task_fft_4_10.job_id = 10U;
  ctx_Task_fft_4_10.node = &node_Task_fft_4_10;

  /* Node Task_fft*5 */
  memset(&node_Task_fft_5_11, 0, sizeof(DagNode));
  node_Task_fft_5_11.name = "Task_fft*5";
  node_Task_fft_5_11.run = vCuNodeWork;
  node_Task_fft_5_11.arg = &ctx_Task_fft_5_11;
  node_Task_fft_5_11.storageSizeBytes = 1920U;
  node_Task_fft_5_11.indegree = 2U;
  node_Task_fft_5_11.mode = DAG_NODE_ASYNC;
  node_Task_fft_5_11.numInputs = 2U;
  node_Task_fft_5_11.inputs = inputs_Task_fft_5_11;
  inputs_Task_fft_5_11[0] = &data_Task_ifft_5_5_out0;
  inputs_Task_fft_5_11[1] = &data_Task_ifft_5_5_out1;
  ctx_Task_fft_5_11.cu_id = CU_ANY_ID;
  ctx_Task_fft_5_11.job_id = 11U;
  ctx_Task_fft_5_11.node = &node_Task_fft_5_11;

}

void vStartDagDemo(void) {
  vDagInit(); vCuInit();
  DagRuntimeConfig cfg = { .workerCount = NUM_WORKERS, .workerPriority = tskIDLE_PRIORITY + 2,
    .workerStackWords = 512, .asyncPriority = tskIDLE_PRIORITY + 3, .asyncStackWords = 512,
    .readyQueueLen = DAG_QUEUE_LEN, .asyncDoneQueueLen = DAG_ASYNC_DONE_QUEUE_LEN, .useCoreAffinity = 1 };
  vDagRuntimeStart(&cfg);
  vDagSubmitReadyNode(&node_Task_ifft_0);
  vDagSubmitReadyNode(&node_Task_ifft_1_1);
  vDagSubmitReadyNode(&node_Task_ifft_2_2);
  vDagSubmitReadyNode(&node_Task_ifft_3_3);
  vDagSubmitReadyNode(&node_Task_ifft_4_4);
  vDagSubmitReadyNode(&node_Task_ifft_5_5);
}