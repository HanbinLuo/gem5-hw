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
#define DAG_QUEUE_LEN 1000
#define DAG_ASYNC_DONE_QUEUE_LEN 1000

typedef struct { uint32_t cu_id; uint32_t job_id; DagNode* node; } CuNodeCtx_t;

static DagNode node_Task_A_0;
static DagNode node_Task_B_1;
static DagNode node_Task_C_2;
static DagNode node_Task_D_3;
static DagNode node_Task_E_4;
static DagNode node_Task_F_5;
static DagNode node_Task_G_6;
static DagNode node_Task_H_7;
static DagNode node_Task_I_8;
static DagNode node_Task_J_9;

/* External/Constant Data Objects */
static DagData extdata_A_input; /* A_input, size=512 */

/* Task-to-Task Data Objects */
static DagData data_A_to_B; /* A_to_B, size=512 */
static DagData data_B_to_C; /* B_to_C, size=512 */
static DagData data_C_to_D; /* C_to_D, size=512 */
static DagData data_D_to_E; /* D_to_E, size=512 */
static DagData data_E_to_F; /* E_to_F, size=512 */
static DagData data_F_to_G; /* F_to_G, size=512 */
static DagData data_G_to_H; /* G_to_H, size=512 */
static DagData data_H_to_I; /* H_to_I, size=512 */
static DagData data_I_to_J; /* I_to_J, size=512 */

static CuNodeCtx_t ctx_Task_A_0;
static CuNodeCtx_t ctx_Task_B_1;
static CuNodeCtx_t ctx_Task_C_2;
static CuNodeCtx_t ctx_Task_D_3;
static CuNodeCtx_t ctx_Task_E_4;
static CuNodeCtx_t ctx_Task_F_5;
static CuNodeCtx_t ctx_Task_G_6;
static CuNodeCtx_t ctx_Task_H_7;
static CuNodeCtx_t ctx_Task_I_8;
static CuNodeCtx_t ctx_Task_J_9;

static DagNode* succOf_Task_A_0[1];
static DagData* inputs_Task_A_0[1];
static DagData* outputs_Task_A_0[1];
static DagNode* succOf_Task_B_1[1];
static DagData* inputs_Task_B_1[1];
static DagData* outputs_Task_B_1[1];
static DagNode* succOf_Task_C_2[1];
static DagData* inputs_Task_C_2[1];
static DagData* outputs_Task_C_2[1];
static DagNode* succOf_Task_D_3[1];
static DagData* inputs_Task_D_3[1];
static DagData* outputs_Task_D_3[1];
static DagNode* succOf_Task_E_4[1];
static DagData* inputs_Task_E_4[1];
static DagData* outputs_Task_E_4[1];
static DagNode* succOf_Task_F_5[1];
static DagData* inputs_Task_F_5[1];
static DagData* outputs_Task_F_5[1];
static DagNode* succOf_Task_G_6[1];
static DagData* inputs_Task_G_6[1];
static DagData* outputs_Task_G_6[1];
static DagNode* succOf_Task_H_7[1];
static DagData* inputs_Task_H_7[1];
static DagData* outputs_Task_H_7[1];
static DagNode* succOf_Task_I_8[1];
static DagData* inputs_Task_I_8[1];
static DagData* outputs_Task_I_8[1];
static DagData* inputs_Task_J_9[1];

static void vCuNodeWork(void* arg) {
  CuNodeCtx_t* ctx = (CuNodeCtx_t*)arg;
  ctx->node->async_pending = 1U;
  vCuSubmitDagJob(ctx->cu_id, ctx->job_id, ctx->node);
}

static void vDagInit(void) {
  /* Initialize External/Constant Data Objects */
  memset(&extdata_A_input, 0, sizeof(DagData));
  extdata_A_input.name = "A_input";
  extdata_A_input.sizeBytes = 512U;
  extdata_A_input.consumers = 1U;

  /* Initialize Task-to-Task Data Objects */
  memset(&data_A_to_B, 0, sizeof(DagData));
  data_A_to_B.name = "A_to_B";
  data_A_to_B.sizeBytes = 512U;
  data_A_to_B.consumers = 1U;
  memset(&data_B_to_C, 0, sizeof(DagData));
  data_B_to_C.name = "B_to_C";
  data_B_to_C.sizeBytes = 512U;
  data_B_to_C.consumers = 1U;
  memset(&data_C_to_D, 0, sizeof(DagData));
  data_C_to_D.name = "C_to_D";
  data_C_to_D.sizeBytes = 512U;
  data_C_to_D.consumers = 1U;
  memset(&data_D_to_E, 0, sizeof(DagData));
  data_D_to_E.name = "D_to_E";
  data_D_to_E.sizeBytes = 512U;
  data_D_to_E.consumers = 1U;
  memset(&data_E_to_F, 0, sizeof(DagData));
  data_E_to_F.name = "E_to_F";
  data_E_to_F.sizeBytes = 512U;
  data_E_to_F.consumers = 1U;
  memset(&data_F_to_G, 0, sizeof(DagData));
  data_F_to_G.name = "F_to_G";
  data_F_to_G.sizeBytes = 512U;
  data_F_to_G.consumers = 1U;
  memset(&data_G_to_H, 0, sizeof(DagData));
  data_G_to_H.name = "G_to_H";
  data_G_to_H.sizeBytes = 512U;
  data_G_to_H.consumers = 1U;
  memset(&data_H_to_I, 0, sizeof(DagData));
  data_H_to_I.name = "H_to_I";
  data_H_to_I.sizeBytes = 512U;
  data_H_to_I.consumers = 1U;
  memset(&data_I_to_J, 0, sizeof(DagData));
  data_I_to_J.name = "I_to_J";
  data_I_to_J.sizeBytes = 512U;
  data_I_to_J.consumers = 1U;

  /* Node Task_A */
  memset(&node_Task_A_0, 0, sizeof(DagNode));
  node_Task_A_0.name = "Task_A";
  node_Task_A_0.run = vCuNodeWork;
  node_Task_A_0.arg = &ctx_Task_A_0;
  node_Task_A_0.computeDelayMs = 100000U;
  node_Task_A_0.indegree = 0U;
  node_Task_A_0.mode = DAG_NODE_ASYNC;
  node_Task_A_0.numInputs = 1U;
  node_Task_A_0.inputs = inputs_Task_A_0;
  inputs_Task_A_0[0] = &extdata_A_input; /* A_input, len=512 */
  node_Task_A_0.numOutputs = 1U;
  node_Task_A_0.outputs = outputs_Task_A_0;
  outputs_Task_A_0[0] = &data_A_to_B;
  node_Task_A_0.numSuccessors = 1U;
  node_Task_A_0.successors = succOf_Task_A_0;
  succOf_Task_A_0[0] = &node_Task_B_1;
  ctx_Task_A_0.cu_id = CU_ANY_ID;
  ctx_Task_A_0.job_id = 0U;
  ctx_Task_A_0.node = &node_Task_A_0;

  /* Node Task_B */
  memset(&node_Task_B_1, 0, sizeof(DagNode));
  node_Task_B_1.name = "Task_B";
  node_Task_B_1.run = vCuNodeWork;
  node_Task_B_1.arg = &ctx_Task_B_1;
  node_Task_B_1.computeDelayMs = 100000U;
  node_Task_B_1.indegree = 1U;
  node_Task_B_1.mode = DAG_NODE_ASYNC;
  node_Task_B_1.numInputs = 1U;
  node_Task_B_1.inputs = inputs_Task_B_1;
  inputs_Task_B_1[0] = &data_A_to_B; /* A_to_B, len=512 */
  node_Task_B_1.numOutputs = 1U;
  node_Task_B_1.outputs = outputs_Task_B_1;
  outputs_Task_B_1[0] = &data_B_to_C;
  node_Task_B_1.numSuccessors = 1U;
  node_Task_B_1.successors = succOf_Task_B_1;
  succOf_Task_B_1[0] = &node_Task_C_2;
  ctx_Task_B_1.cu_id = CU_ANY_ID;
  ctx_Task_B_1.job_id = 1U;
  ctx_Task_B_1.node = &node_Task_B_1;

  /* Node Task_C */
  memset(&node_Task_C_2, 0, sizeof(DagNode));
  node_Task_C_2.name = "Task_C";
  node_Task_C_2.run = vCuNodeWork;
  node_Task_C_2.arg = &ctx_Task_C_2;
  node_Task_C_2.computeDelayMs = 100000U;
  node_Task_C_2.indegree = 1U;
  node_Task_C_2.mode = DAG_NODE_ASYNC;
  node_Task_C_2.numInputs = 1U;
  node_Task_C_2.inputs = inputs_Task_C_2;
  inputs_Task_C_2[0] = &data_B_to_C; /* B_to_C, len=512 */
  node_Task_C_2.numOutputs = 1U;
  node_Task_C_2.outputs = outputs_Task_C_2;
  outputs_Task_C_2[0] = &data_C_to_D;
  node_Task_C_2.numSuccessors = 1U;
  node_Task_C_2.successors = succOf_Task_C_2;
  succOf_Task_C_2[0] = &node_Task_D_3;
  ctx_Task_C_2.cu_id = CU_ANY_ID;
  ctx_Task_C_2.job_id = 2U;
  ctx_Task_C_2.node = &node_Task_C_2;

  /* Node Task_D */
  memset(&node_Task_D_3, 0, sizeof(DagNode));
  node_Task_D_3.name = "Task_D";
  node_Task_D_3.run = vCuNodeWork;
  node_Task_D_3.arg = &ctx_Task_D_3;
  node_Task_D_3.computeDelayMs = 100000U;
  node_Task_D_3.indegree = 1U;
  node_Task_D_3.mode = DAG_NODE_ASYNC;
  node_Task_D_3.numInputs = 1U;
  node_Task_D_3.inputs = inputs_Task_D_3;
  inputs_Task_D_3[0] = &data_C_to_D; /* C_to_D, len=512 */
  node_Task_D_3.numOutputs = 1U;
  node_Task_D_3.outputs = outputs_Task_D_3;
  outputs_Task_D_3[0] = &data_D_to_E;
  node_Task_D_3.numSuccessors = 1U;
  node_Task_D_3.successors = succOf_Task_D_3;
  succOf_Task_D_3[0] = &node_Task_E_4;
  ctx_Task_D_3.cu_id = CU_ANY_ID;
  ctx_Task_D_3.job_id = 3U;
  ctx_Task_D_3.node = &node_Task_D_3;

  /* Node Task_E */
  memset(&node_Task_E_4, 0, sizeof(DagNode));
  node_Task_E_4.name = "Task_E";
  node_Task_E_4.run = vCuNodeWork;
  node_Task_E_4.arg = &ctx_Task_E_4;
  node_Task_E_4.computeDelayMs = 100000U;
  node_Task_E_4.indegree = 1U;
  node_Task_E_4.mode = DAG_NODE_ASYNC;
  node_Task_E_4.numInputs = 1U;
  node_Task_E_4.inputs = inputs_Task_E_4;
  inputs_Task_E_4[0] = &data_D_to_E; /* D_to_E, len=512 */
  node_Task_E_4.numOutputs = 1U;
  node_Task_E_4.outputs = outputs_Task_E_4;
  outputs_Task_E_4[0] = &data_E_to_F;
  node_Task_E_4.numSuccessors = 1U;
  node_Task_E_4.successors = succOf_Task_E_4;
  succOf_Task_E_4[0] = &node_Task_F_5;
  ctx_Task_E_4.cu_id = CU_ANY_ID;
  ctx_Task_E_4.job_id = 4U;
  ctx_Task_E_4.node = &node_Task_E_4;

  /* Node Task_F */
  memset(&node_Task_F_5, 0, sizeof(DagNode));
  node_Task_F_5.name = "Task_F";
  node_Task_F_5.run = vCuNodeWork;
  node_Task_F_5.arg = &ctx_Task_F_5;
  node_Task_F_5.computeDelayMs = 100000U;
  node_Task_F_5.indegree = 1U;
  node_Task_F_5.mode = DAG_NODE_ASYNC;
  node_Task_F_5.numInputs = 1U;
  node_Task_F_5.inputs = inputs_Task_F_5;
  inputs_Task_F_5[0] = &data_E_to_F; /* E_to_F, len=512 */
  node_Task_F_5.numOutputs = 1U;
  node_Task_F_5.outputs = outputs_Task_F_5;
  outputs_Task_F_5[0] = &data_F_to_G;
  node_Task_F_5.numSuccessors = 1U;
  node_Task_F_5.successors = succOf_Task_F_5;
  succOf_Task_F_5[0] = &node_Task_G_6;
  ctx_Task_F_5.cu_id = CU_ANY_ID;
  ctx_Task_F_5.job_id = 5U;
  ctx_Task_F_5.node = &node_Task_F_5;

  /* Node Task_G */
  memset(&node_Task_G_6, 0, sizeof(DagNode));
  node_Task_G_6.name = "Task_G";
  node_Task_G_6.run = vCuNodeWork;
  node_Task_G_6.arg = &ctx_Task_G_6;
  node_Task_G_6.computeDelayMs = 100000U;
  node_Task_G_6.indegree = 1U;
  node_Task_G_6.mode = DAG_NODE_ASYNC;
  node_Task_G_6.numInputs = 1U;
  node_Task_G_6.inputs = inputs_Task_G_6;
  inputs_Task_G_6[0] = &data_F_to_G; /* F_to_G, len=512 */
  node_Task_G_6.numOutputs = 1U;
  node_Task_G_6.outputs = outputs_Task_G_6;
  outputs_Task_G_6[0] = &data_G_to_H;
  node_Task_G_6.numSuccessors = 1U;
  node_Task_G_6.successors = succOf_Task_G_6;
  succOf_Task_G_6[0] = &node_Task_H_7;
  ctx_Task_G_6.cu_id = CU_ANY_ID;
  ctx_Task_G_6.job_id = 6U;
  ctx_Task_G_6.node = &node_Task_G_6;

  /* Node Task_H */
  memset(&node_Task_H_7, 0, sizeof(DagNode));
  node_Task_H_7.name = "Task_H";
  node_Task_H_7.run = vCuNodeWork;
  node_Task_H_7.arg = &ctx_Task_H_7;
  node_Task_H_7.computeDelayMs = 100000U;
  node_Task_H_7.indegree = 1U;
  node_Task_H_7.mode = DAG_NODE_ASYNC;
  node_Task_H_7.numInputs = 1U;
  node_Task_H_7.inputs = inputs_Task_H_7;
  inputs_Task_H_7[0] = &data_G_to_H; /* G_to_H, len=512 */
  node_Task_H_7.numOutputs = 1U;
  node_Task_H_7.outputs = outputs_Task_H_7;
  outputs_Task_H_7[0] = &data_H_to_I;
  node_Task_H_7.numSuccessors = 1U;
  node_Task_H_7.successors = succOf_Task_H_7;
  succOf_Task_H_7[0] = &node_Task_I_8;
  ctx_Task_H_7.cu_id = CU_ANY_ID;
  ctx_Task_H_7.job_id = 7U;
  ctx_Task_H_7.node = &node_Task_H_7;

  /* Node Task_I */
  memset(&node_Task_I_8, 0, sizeof(DagNode));
  node_Task_I_8.name = "Task_I";
  node_Task_I_8.run = vCuNodeWork;
  node_Task_I_8.arg = &ctx_Task_I_8;
  node_Task_I_8.computeDelayMs = 100000U;
  node_Task_I_8.indegree = 1U;
  node_Task_I_8.mode = DAG_NODE_ASYNC;
  node_Task_I_8.numInputs = 1U;
  node_Task_I_8.inputs = inputs_Task_I_8;
  inputs_Task_I_8[0] = &data_H_to_I; /* H_to_I, len=512 */
  node_Task_I_8.numOutputs = 1U;
  node_Task_I_8.outputs = outputs_Task_I_8;
  outputs_Task_I_8[0] = &data_I_to_J;
  node_Task_I_8.numSuccessors = 1U;
  node_Task_I_8.successors = succOf_Task_I_8;
  succOf_Task_I_8[0] = &node_Task_J_9;
  ctx_Task_I_8.cu_id = CU_ANY_ID;
  ctx_Task_I_8.job_id = 8U;
  ctx_Task_I_8.node = &node_Task_I_8;

  /* Node Task_J */
  memset(&node_Task_J_9, 0, sizeof(DagNode));
  node_Task_J_9.name = "Task_J";
  node_Task_J_9.run = vCuNodeWork;
  node_Task_J_9.arg = &ctx_Task_J_9;
  node_Task_J_9.computeDelayMs = 100000U;
  node_Task_J_9.indegree = 1U;
  node_Task_J_9.mode = DAG_NODE_ASYNC;
  node_Task_J_9.numInputs = 1U;
  node_Task_J_9.inputs = inputs_Task_J_9;
  inputs_Task_J_9[0] = &data_I_to_J; /* I_to_J, len=512 */
  ctx_Task_J_9.cu_id = CU_ANY_ID;
  ctx_Task_J_9.job_id = 9U;
  ctx_Task_J_9.node = &node_Task_J_9;

}

static void vDagSubmitInitialNodes(void) {
  vDagSubmitReadyNode(&node_Task_A_0);
}

static void vDagKickoffTask(void* pvParameters) {
  (void)pvParameters;
  vDagSubmitInitialNodes();
  vTaskDelete(NULL);
}

void vStartDagDemo(void) {
  vDagInit(); vCuInit();
  vDagSetTotalNodes(10U);
  DagRuntimeConfig cfg = { .workerCount = NUM_WORKERS, .workerPriority = tskIDLE_PRIORITY + 2,
    .workerStackWords = 512, .asyncPriority = tskIDLE_PRIORITY + 3, .asyncStackWords = 512,
    .readyQueueLen = DAG_QUEUE_LEN, .asyncDoneQueueLen = DAG_ASYNC_DONE_QUEUE_LEN, .useCoreAffinity = 1 };
  vDagRuntimeStart(&cfg);
  configASSERT(xTaskCreate(vDagKickoffTask, "DagKick", 512, NULL,
                           tskIDLE_PRIORITY + 1, NULL) == pdPASS);
}
