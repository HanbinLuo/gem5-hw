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

/* External/Constant Data Objects */
static DagData extdata_cu_input_real_0_0; /* cu_input_real_0_0, size=256 */
static DagData extdata_cu_input_imag_0_0; /* cu_input_imag_0_0, size=256 */
static DagData extdata_zc_real_0; /* zc_real_0, size=256 */
static DagData extdata_zc_imag_0; /* zc_imag_0, size=256 */
static DagData extdata_cos_stage3; /* cos_stage3, size=256 */
static DagData extdata_cos_stage4; /* cos_stage4, size=256 */
static DagData extdata_cos_stage5; /* cos_stage5, size=256 */
static DagData extdata_cos_stage6; /* cos_stage6, size=256 */
static DagData extdata_cos_stage7; /* cos_stage7, size=256 */
static DagData extdata_cos_stage8; /* cos_stage8, size=256 */
static DagData extdata_cos_stage9; /* cos_stage9, size=256 */
static DagData extdata_cos_stage10; /* cos_stage10, size=256 */
static DagData extdata_sin_stage3; /* sin_stage3, size=256 */
static DagData extdata_sin_stage4; /* sin_stage4, size=256 */
static DagData extdata_sin_stage5; /* sin_stage5, size=256 */
static DagData extdata_sin_stage6; /* sin_stage6, size=256 */
static DagData extdata_sin_stage7; /* sin_stage7, size=256 */
static DagData extdata_sin_stage8; /* sin_stage8, size=256 */
static DagData extdata_sin_stage9; /* sin_stage9, size=256 */
static DagData extdata_sin_stage10; /* sin_stage10, size=256 */
static DagData extdata_shuffle_add_stage0; /* shuffle_add_stage0, size=512 */
static DagData extdata_shuffle_add_stage1; /* shuffle_add_stage1, size=512 */
static DagData extdata_shuffle_add_stage2; /* shuffle_add_stage2, size=512 */
static DagData extdata_shuffle_add_stage3; /* shuffle_add_stage3, size=512 */
static DagData extdata_shuffle_add_stage4; /* shuffle_add_stage4, size=512 */
static DagData extdata_shuffle_add_stage5; /* shuffle_add_stage5, size=512 */
static DagData extdata_shuffle_add_stage6; /* shuffle_add_stage6, size=512 */
static DagData extdata_shuffle_add_stage7; /* shuffle_add_stage7, size=512 */
static DagData extdata_shuffle_wn_stage0; /* shuffle_wn_stage0, size=512 */
static DagData extdata_shuffle_wn_stage1; /* shuffle_wn_stage1, size=512 */
static DagData extdata_shuffle_wn_stage2; /* shuffle_wn_stage2, size=512 */
static DagData extdata_shuffle_wn_stage3; /* shuffle_wn_stage3, size=512 */
static DagData extdata_shuffle_wn_stage4; /* shuffle_wn_stage4, size=512 */
static DagData extdata_shuffle_wn_stage5; /* shuffle_wn_stage5, size=512 */
static DagData extdata_shuffle_wn_stage6; /* shuffle_wn_stage6, size=512 */
static DagData extdata_shuffle_wn_stage7; /* shuffle_wn_stage7, size=512 */
static DagData extdata_cu_input_real_0_1; /* cu_input_real_0_1, size=256 */
static DagData extdata_cu_input_imag_0_1; /* cu_input_imag_0_1, size=256 */
static DagData extdata_zc_real_1; /* zc_real_1, size=256 */
static DagData extdata_zc_imag_1; /* zc_imag_1, size=256 */
static DagData extdata_cu_input_real_1_0; /* cu_input_real_1_0, size=256 */
static DagData extdata_cu_input_imag_1_0; /* cu_input_imag_1_0, size=256 */
static DagData extdata_cu_input_real_1_1; /* cu_input_real_1_1, size=256 */
static DagData extdata_cu_input_imag_1_1; /* cu_input_imag_1_1, size=256 */
static DagData extdata_cu_input_real_2_0; /* cu_input_real_2_0, size=256 */
static DagData extdata_cu_input_imag_2_0; /* cu_input_imag_2_0, size=256 */
static DagData extdata_cu_input_real_2_1; /* cu_input_real_2_1, size=256 */
static DagData extdata_cu_input_imag_2_1; /* cu_input_imag_2_1, size=256 */

/* Task-to-Task Data Objects */
static DagData data_ifft_output_real_0_0; /* ifft_output_real_0_0, size=64 */
static DagData data_ifft_output_imag_0_0; /* ifft_output_imag_0_0, size=64 */
static DagData data_ifft_output_real_0_1; /* ifft_output_real_0_1, size=64 */
static DagData data_ifft_output_imag_0_1; /* ifft_output_imag_0_1, size=64 */
static DagData data_ifft_output_real_1_0; /* ifft_output_real_1_0, size=64 */
static DagData data_ifft_output_imag_1_0; /* ifft_output_imag_1_0, size=64 */
static DagData data_ifft_output_real_1_1; /* ifft_output_real_1_1, size=64 */
static DagData data_ifft_output_imag_1_1; /* ifft_output_imag_1_1, size=64 */
static DagData data_ifft_output_real_2_0; /* ifft_output_real_2_0, size=64 */
static DagData data_ifft_output_imag_2_0; /* ifft_output_imag_2_0, size=64 */
static DagData data_ifft_output_real_2_1; /* ifft_output_real_2_1, size=64 */
static DagData data_ifft_output_imag_2_1; /* ifft_output_imag_2_1, size=64 */

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
static DagData* inputs_Task_ifft_0[36];
static DagData* outputs_Task_ifft_0[2];
static DagNode* succOf_Task_ifft_1_1[1];
static DagData* inputs_Task_ifft_1_1[36];
static DagData* outputs_Task_ifft_1_1[2];
static DagNode* succOf_Task_ifft_2_2[1];
static DagData* inputs_Task_ifft_2_2[36];
static DagData* outputs_Task_ifft_2_2[2];
static DagNode* succOf_Task_ifft_3_3[1];
static DagData* inputs_Task_ifft_3_3[36];
static DagData* outputs_Task_ifft_3_3[2];
static DagNode* succOf_Task_ifft_4_4[1];
static DagData* inputs_Task_ifft_4_4[36];
static DagData* outputs_Task_ifft_4_4[2];
static DagNode* succOf_Task_ifft_5_5[1];
static DagData* inputs_Task_ifft_5_5[36];
static DagData* outputs_Task_ifft_5_5[2];
static DagData* inputs_Task_fft_6[34];
static DagData* inputs_Task_fft_1_7[34];
static DagData* inputs_Task_fft_2_8[34];
static DagData* inputs_Task_fft_3_9[34];
static DagData* inputs_Task_fft_4_10[34];
static DagData* inputs_Task_fft_5_11[34];

static void vCuNodeWork(void* arg) {
  CuNodeCtx_t* ctx = (CuNodeCtx_t*)arg;
  ctx->node->async_pending = 1U;
  vCuSubmitDagJob(ctx->cu_id, ctx->job_id, ctx->node);
}

static void vDagInit(void) {
  /* Initialize External/Constant Data Objects */
  memset(&extdata_cu_input_real_0_0, 0, sizeof(DagData));
  extdata_cu_input_real_0_0.name = "cu_input_real_0_0";
  extdata_cu_input_real_0_0.sizeBytes = 256U;
  extdata_cu_input_real_0_0.consumers = 1U;
  memset(&extdata_cu_input_imag_0_0, 0, sizeof(DagData));
  extdata_cu_input_imag_0_0.name = "cu_input_imag_0_0";
  extdata_cu_input_imag_0_0.sizeBytes = 256U;
  extdata_cu_input_imag_0_0.consumers = 1U;
  memset(&extdata_zc_real_0, 0, sizeof(DagData));
  extdata_zc_real_0.name = "zc_real_0";
  extdata_zc_real_0.sizeBytes = 256U;
  extdata_zc_real_0.consumers = 3U;
  memset(&extdata_zc_imag_0, 0, sizeof(DagData));
  extdata_zc_imag_0.name = "zc_imag_0";
  extdata_zc_imag_0.sizeBytes = 256U;
  extdata_zc_imag_0.consumers = 3U;
  memset(&extdata_cos_stage3, 0, sizeof(DagData));
  extdata_cos_stage3.name = "cos_stage3";
  extdata_cos_stage3.sizeBytes = 256U;
  extdata_cos_stage3.consumers = 12U;
  memset(&extdata_cos_stage4, 0, sizeof(DagData));
  extdata_cos_stage4.name = "cos_stage4";
  extdata_cos_stage4.sizeBytes = 256U;
  extdata_cos_stage4.consumers = 12U;
  memset(&extdata_cos_stage5, 0, sizeof(DagData));
  extdata_cos_stage5.name = "cos_stage5";
  extdata_cos_stage5.sizeBytes = 256U;
  extdata_cos_stage5.consumers = 12U;
  memset(&extdata_cos_stage6, 0, sizeof(DagData));
  extdata_cos_stage6.name = "cos_stage6";
  extdata_cos_stage6.sizeBytes = 256U;
  extdata_cos_stage6.consumers = 12U;
  memset(&extdata_cos_stage7, 0, sizeof(DagData));
  extdata_cos_stage7.name = "cos_stage7";
  extdata_cos_stage7.sizeBytes = 256U;
  extdata_cos_stage7.consumers = 12U;
  memset(&extdata_cos_stage8, 0, sizeof(DagData));
  extdata_cos_stage8.name = "cos_stage8";
  extdata_cos_stage8.sizeBytes = 256U;
  extdata_cos_stage8.consumers = 12U;
  memset(&extdata_cos_stage9, 0, sizeof(DagData));
  extdata_cos_stage9.name = "cos_stage9";
  extdata_cos_stage9.sizeBytes = 256U;
  extdata_cos_stage9.consumers = 12U;
  memset(&extdata_cos_stage10, 0, sizeof(DagData));
  extdata_cos_stage10.name = "cos_stage10";
  extdata_cos_stage10.sizeBytes = 256U;
  extdata_cos_stage10.consumers = 12U;
  memset(&extdata_sin_stage3, 0, sizeof(DagData));
  extdata_sin_stage3.name = "sin_stage3";
  extdata_sin_stage3.sizeBytes = 256U;
  extdata_sin_stage3.consumers = 12U;
  memset(&extdata_sin_stage4, 0, sizeof(DagData));
  extdata_sin_stage4.name = "sin_stage4";
  extdata_sin_stage4.sizeBytes = 256U;
  extdata_sin_stage4.consumers = 12U;
  memset(&extdata_sin_stage5, 0, sizeof(DagData));
  extdata_sin_stage5.name = "sin_stage5";
  extdata_sin_stage5.sizeBytes = 256U;
  extdata_sin_stage5.consumers = 12U;
  memset(&extdata_sin_stage6, 0, sizeof(DagData));
  extdata_sin_stage6.name = "sin_stage6";
  extdata_sin_stage6.sizeBytes = 256U;
  extdata_sin_stage6.consumers = 12U;
  memset(&extdata_sin_stage7, 0, sizeof(DagData));
  extdata_sin_stage7.name = "sin_stage7";
  extdata_sin_stage7.sizeBytes = 256U;
  extdata_sin_stage7.consumers = 12U;
  memset(&extdata_sin_stage8, 0, sizeof(DagData));
  extdata_sin_stage8.name = "sin_stage8";
  extdata_sin_stage8.sizeBytes = 256U;
  extdata_sin_stage8.consumers = 12U;
  memset(&extdata_sin_stage9, 0, sizeof(DagData));
  extdata_sin_stage9.name = "sin_stage9";
  extdata_sin_stage9.sizeBytes = 256U;
  extdata_sin_stage9.consumers = 12U;
  memset(&extdata_sin_stage10, 0, sizeof(DagData));
  extdata_sin_stage10.name = "sin_stage10";
  extdata_sin_stage10.sizeBytes = 256U;
  extdata_sin_stage10.consumers = 12U;
  memset(&extdata_shuffle_add_stage0, 0, sizeof(DagData));
  extdata_shuffle_add_stage0.name = "shuffle_add_stage0";
  extdata_shuffle_add_stage0.sizeBytes = 512U;
  extdata_shuffle_add_stage0.consumers = 12U;
  memset(&extdata_shuffle_add_stage1, 0, sizeof(DagData));
  extdata_shuffle_add_stage1.name = "shuffle_add_stage1";
  extdata_shuffle_add_stage1.sizeBytes = 512U;
  extdata_shuffle_add_stage1.consumers = 12U;
  memset(&extdata_shuffle_add_stage2, 0, sizeof(DagData));
  extdata_shuffle_add_stage2.name = "shuffle_add_stage2";
  extdata_shuffle_add_stage2.sizeBytes = 512U;
  extdata_shuffle_add_stage2.consumers = 12U;
  memset(&extdata_shuffle_add_stage3, 0, sizeof(DagData));
  extdata_shuffle_add_stage3.name = "shuffle_add_stage3";
  extdata_shuffle_add_stage3.sizeBytes = 512U;
  extdata_shuffle_add_stage3.consumers = 12U;
  memset(&extdata_shuffle_add_stage4, 0, sizeof(DagData));
  extdata_shuffle_add_stage4.name = "shuffle_add_stage4";
  extdata_shuffle_add_stage4.sizeBytes = 512U;
  extdata_shuffle_add_stage4.consumers = 12U;
  memset(&extdata_shuffle_add_stage5, 0, sizeof(DagData));
  extdata_shuffle_add_stage5.name = "shuffle_add_stage5";
  extdata_shuffle_add_stage5.sizeBytes = 512U;
  extdata_shuffle_add_stage5.consumers = 12U;
  memset(&extdata_shuffle_add_stage6, 0, sizeof(DagData));
  extdata_shuffle_add_stage6.name = "shuffle_add_stage6";
  extdata_shuffle_add_stage6.sizeBytes = 512U;
  extdata_shuffle_add_stage6.consumers = 12U;
  memset(&extdata_shuffle_add_stage7, 0, sizeof(DagData));
  extdata_shuffle_add_stage7.name = "shuffle_add_stage7";
  extdata_shuffle_add_stage7.sizeBytes = 512U;
  extdata_shuffle_add_stage7.consumers = 12U;
  memset(&extdata_shuffle_wn_stage0, 0, sizeof(DagData));
  extdata_shuffle_wn_stage0.name = "shuffle_wn_stage0";
  extdata_shuffle_wn_stage0.sizeBytes = 512U;
  extdata_shuffle_wn_stage0.consumers = 12U;
  memset(&extdata_shuffle_wn_stage1, 0, sizeof(DagData));
  extdata_shuffle_wn_stage1.name = "shuffle_wn_stage1";
  extdata_shuffle_wn_stage1.sizeBytes = 512U;
  extdata_shuffle_wn_stage1.consumers = 12U;
  memset(&extdata_shuffle_wn_stage2, 0, sizeof(DagData));
  extdata_shuffle_wn_stage2.name = "shuffle_wn_stage2";
  extdata_shuffle_wn_stage2.sizeBytes = 512U;
  extdata_shuffle_wn_stage2.consumers = 12U;
  memset(&extdata_shuffle_wn_stage3, 0, sizeof(DagData));
  extdata_shuffle_wn_stage3.name = "shuffle_wn_stage3";
  extdata_shuffle_wn_stage3.sizeBytes = 512U;
  extdata_shuffle_wn_stage3.consumers = 12U;
  memset(&extdata_shuffle_wn_stage4, 0, sizeof(DagData));
  extdata_shuffle_wn_stage4.name = "shuffle_wn_stage4";
  extdata_shuffle_wn_stage4.sizeBytes = 512U;
  extdata_shuffle_wn_stage4.consumers = 12U;
  memset(&extdata_shuffle_wn_stage5, 0, sizeof(DagData));
  extdata_shuffle_wn_stage5.name = "shuffle_wn_stage5";
  extdata_shuffle_wn_stage5.sizeBytes = 512U;
  extdata_shuffle_wn_stage5.consumers = 12U;
  memset(&extdata_shuffle_wn_stage6, 0, sizeof(DagData));
  extdata_shuffle_wn_stage6.name = "shuffle_wn_stage6";
  extdata_shuffle_wn_stage6.sizeBytes = 512U;
  extdata_shuffle_wn_stage6.consumers = 12U;
  memset(&extdata_shuffle_wn_stage7, 0, sizeof(DagData));
  extdata_shuffle_wn_stage7.name = "shuffle_wn_stage7";
  extdata_shuffle_wn_stage7.sizeBytes = 512U;
  extdata_shuffle_wn_stage7.consumers = 12U;
  memset(&extdata_cu_input_real_0_1, 0, sizeof(DagData));
  extdata_cu_input_real_0_1.name = "cu_input_real_0_1";
  extdata_cu_input_real_0_1.sizeBytes = 256U;
  extdata_cu_input_real_0_1.consumers = 1U;
  memset(&extdata_cu_input_imag_0_1, 0, sizeof(DagData));
  extdata_cu_input_imag_0_1.name = "cu_input_imag_0_1";
  extdata_cu_input_imag_0_1.sizeBytes = 256U;
  extdata_cu_input_imag_0_1.consumers = 1U;
  memset(&extdata_zc_real_1, 0, sizeof(DagData));
  extdata_zc_real_1.name = "zc_real_1";
  extdata_zc_real_1.sizeBytes = 256U;
  extdata_zc_real_1.consumers = 3U;
  memset(&extdata_zc_imag_1, 0, sizeof(DagData));
  extdata_zc_imag_1.name = "zc_imag_1";
  extdata_zc_imag_1.sizeBytes = 256U;
  extdata_zc_imag_1.consumers = 3U;
  memset(&extdata_cu_input_real_1_0, 0, sizeof(DagData));
  extdata_cu_input_real_1_0.name = "cu_input_real_1_0";
  extdata_cu_input_real_1_0.sizeBytes = 256U;
  extdata_cu_input_real_1_0.consumers = 1U;
  memset(&extdata_cu_input_imag_1_0, 0, sizeof(DagData));
  extdata_cu_input_imag_1_0.name = "cu_input_imag_1_0";
  extdata_cu_input_imag_1_0.sizeBytes = 256U;
  extdata_cu_input_imag_1_0.consumers = 1U;
  memset(&extdata_cu_input_real_1_1, 0, sizeof(DagData));
  extdata_cu_input_real_1_1.name = "cu_input_real_1_1";
  extdata_cu_input_real_1_1.sizeBytes = 256U;
  extdata_cu_input_real_1_1.consumers = 1U;
  memset(&extdata_cu_input_imag_1_1, 0, sizeof(DagData));
  extdata_cu_input_imag_1_1.name = "cu_input_imag_1_1";
  extdata_cu_input_imag_1_1.sizeBytes = 256U;
  extdata_cu_input_imag_1_1.consumers = 1U;
  memset(&extdata_cu_input_real_2_0, 0, sizeof(DagData));
  extdata_cu_input_real_2_0.name = "cu_input_real_2_0";
  extdata_cu_input_real_2_0.sizeBytes = 256U;
  extdata_cu_input_real_2_0.consumers = 1U;
  memset(&extdata_cu_input_imag_2_0, 0, sizeof(DagData));
  extdata_cu_input_imag_2_0.name = "cu_input_imag_2_0";
  extdata_cu_input_imag_2_0.sizeBytes = 256U;
  extdata_cu_input_imag_2_0.consumers = 1U;
  memset(&extdata_cu_input_real_2_1, 0, sizeof(DagData));
  extdata_cu_input_real_2_1.name = "cu_input_real_2_1";
  extdata_cu_input_real_2_1.sizeBytes = 256U;
  extdata_cu_input_real_2_1.consumers = 1U;
  memset(&extdata_cu_input_imag_2_1, 0, sizeof(DagData));
  extdata_cu_input_imag_2_1.name = "cu_input_imag_2_1";
  extdata_cu_input_imag_2_1.sizeBytes = 256U;
  extdata_cu_input_imag_2_1.consumers = 1U;

  /* Initialize Task-to-Task Data Objects */
  memset(&data_ifft_output_real_0_0, 0, sizeof(DagData));
  data_ifft_output_real_0_0.name = "ifft_output_real_0_0";
  data_ifft_output_real_0_0.sizeBytes = 64U;
  data_ifft_output_real_0_0.consumers = 1U;
  memset(&data_ifft_output_imag_0_0, 0, sizeof(DagData));
  data_ifft_output_imag_0_0.name = "ifft_output_imag_0_0";
  data_ifft_output_imag_0_0.sizeBytes = 64U;
  data_ifft_output_imag_0_0.consumers = 1U;
  memset(&data_ifft_output_real_0_1, 0, sizeof(DagData));
  data_ifft_output_real_0_1.name = "ifft_output_real_0_1";
  data_ifft_output_real_0_1.sizeBytes = 64U;
  data_ifft_output_real_0_1.consumers = 1U;
  memset(&data_ifft_output_imag_0_1, 0, sizeof(DagData));
  data_ifft_output_imag_0_1.name = "ifft_output_imag_0_1";
  data_ifft_output_imag_0_1.sizeBytes = 64U;
  data_ifft_output_imag_0_1.consumers = 1U;
  memset(&data_ifft_output_real_1_0, 0, sizeof(DagData));
  data_ifft_output_real_1_0.name = "ifft_output_real_1_0";
  data_ifft_output_real_1_0.sizeBytes = 64U;
  data_ifft_output_real_1_0.consumers = 1U;
  memset(&data_ifft_output_imag_1_0, 0, sizeof(DagData));
  data_ifft_output_imag_1_0.name = "ifft_output_imag_1_0";
  data_ifft_output_imag_1_0.sizeBytes = 64U;
  data_ifft_output_imag_1_0.consumers = 1U;
  memset(&data_ifft_output_real_1_1, 0, sizeof(DagData));
  data_ifft_output_real_1_1.name = "ifft_output_real_1_1";
  data_ifft_output_real_1_1.sizeBytes = 64U;
  data_ifft_output_real_1_1.consumers = 1U;
  memset(&data_ifft_output_imag_1_1, 0, sizeof(DagData));
  data_ifft_output_imag_1_1.name = "ifft_output_imag_1_1";
  data_ifft_output_imag_1_1.sizeBytes = 64U;
  data_ifft_output_imag_1_1.consumers = 1U;
  memset(&data_ifft_output_real_2_0, 0, sizeof(DagData));
  data_ifft_output_real_2_0.name = "ifft_output_real_2_0";
  data_ifft_output_real_2_0.sizeBytes = 64U;
  data_ifft_output_real_2_0.consumers = 1U;
  memset(&data_ifft_output_imag_2_0, 0, sizeof(DagData));
  data_ifft_output_imag_2_0.name = "ifft_output_imag_2_0";
  data_ifft_output_imag_2_0.sizeBytes = 64U;
  data_ifft_output_imag_2_0.consumers = 1U;
  memset(&data_ifft_output_real_2_1, 0, sizeof(DagData));
  data_ifft_output_real_2_1.name = "ifft_output_real_2_1";
  data_ifft_output_real_2_1.sizeBytes = 64U;
  data_ifft_output_real_2_1.consumers = 1U;
  memset(&data_ifft_output_imag_2_1, 0, sizeof(DagData));
  data_ifft_output_imag_2_1.name = "ifft_output_imag_2_1";
  data_ifft_output_imag_2_1.sizeBytes = 64U;
  data_ifft_output_imag_2_1.consumers = 1U;

  /* Node Task_ifft */
  memset(&node_Task_ifft_0, 0, sizeof(DagNode));
  node_Task_ifft_0.name = "Task_ifft";
  node_Task_ifft_0.run = vCuNodeWork;
  node_Task_ifft_0.arg = &ctx_Task_ifft_0;
  node_Task_ifft_0.computeDelayMs = 204800000U;
  node_Task_ifft_0.indegree = 0U;
  node_Task_ifft_0.mode = DAG_NODE_ASYNC;
  node_Task_ifft_0.numInputs = 36U;
  node_Task_ifft_0.inputs = inputs_Task_ifft_0;
  inputs_Task_ifft_0[0] = &extdata_cu_input_real_0_0; /* cu_input_real_0_0, len=256 */
  inputs_Task_ifft_0[1] = &extdata_cu_input_imag_0_0; /* cu_input_imag_0_0, len=256 */
  inputs_Task_ifft_0[2] = &extdata_zc_real_0; /* zc_real_0, len=256 */
  inputs_Task_ifft_0[3] = &extdata_zc_imag_0; /* zc_imag_0, len=256 */
  inputs_Task_ifft_0[4] = &extdata_cos_stage3; /* cos_stage3, len=256 */
  inputs_Task_ifft_0[5] = &extdata_cos_stage4; /* cos_stage4, len=256 */
  inputs_Task_ifft_0[6] = &extdata_cos_stage5; /* cos_stage5, len=256 */
  inputs_Task_ifft_0[7] = &extdata_cos_stage6; /* cos_stage6, len=256 */
  inputs_Task_ifft_0[8] = &extdata_cos_stage7; /* cos_stage7, len=256 */
  inputs_Task_ifft_0[9] = &extdata_cos_stage8; /* cos_stage8, len=256 */
  inputs_Task_ifft_0[10] = &extdata_cos_stage9; /* cos_stage9, len=256 */
  inputs_Task_ifft_0[11] = &extdata_cos_stage10; /* cos_stage10, len=256 */
  inputs_Task_ifft_0[12] = &extdata_sin_stage3; /* sin_stage3, len=256 */
  inputs_Task_ifft_0[13] = &extdata_sin_stage4; /* sin_stage4, len=256 */
  inputs_Task_ifft_0[14] = &extdata_sin_stage5; /* sin_stage5, len=256 */
  inputs_Task_ifft_0[15] = &extdata_sin_stage6; /* sin_stage6, len=256 */
  inputs_Task_ifft_0[16] = &extdata_sin_stage7; /* sin_stage7, len=256 */
  inputs_Task_ifft_0[17] = &extdata_sin_stage8; /* sin_stage8, len=256 */
  inputs_Task_ifft_0[18] = &extdata_sin_stage9; /* sin_stage9, len=256 */
  inputs_Task_ifft_0[19] = &extdata_sin_stage10; /* sin_stage10, len=256 */
  inputs_Task_ifft_0[20] = &extdata_shuffle_add_stage0; /* shuffle_add_stage0, len=512 */
  inputs_Task_ifft_0[21] = &extdata_shuffle_add_stage1; /* shuffle_add_stage1, len=512 */
  inputs_Task_ifft_0[22] = &extdata_shuffle_add_stage2; /* shuffle_add_stage2, len=512 */
  inputs_Task_ifft_0[23] = &extdata_shuffle_add_stage3; /* shuffle_add_stage3, len=512 */
  inputs_Task_ifft_0[24] = &extdata_shuffle_add_stage4; /* shuffle_add_stage4, len=512 */
  inputs_Task_ifft_0[25] = &extdata_shuffle_add_stage5; /* shuffle_add_stage5, len=512 */
  inputs_Task_ifft_0[26] = &extdata_shuffle_add_stage6; /* shuffle_add_stage6, len=512 */
  inputs_Task_ifft_0[27] = &extdata_shuffle_add_stage7; /* shuffle_add_stage7, len=512 */
  inputs_Task_ifft_0[28] = &extdata_shuffle_wn_stage0; /* shuffle_wn_stage0, len=512 */
  inputs_Task_ifft_0[29] = &extdata_shuffle_wn_stage1; /* shuffle_wn_stage1, len=512 */
  inputs_Task_ifft_0[30] = &extdata_shuffle_wn_stage2; /* shuffle_wn_stage2, len=512 */
  inputs_Task_ifft_0[31] = &extdata_shuffle_wn_stage3; /* shuffle_wn_stage3, len=512 */
  inputs_Task_ifft_0[32] = &extdata_shuffle_wn_stage4; /* shuffle_wn_stage4, len=512 */
  inputs_Task_ifft_0[33] = &extdata_shuffle_wn_stage5; /* shuffle_wn_stage5, len=512 */
  inputs_Task_ifft_0[34] = &extdata_shuffle_wn_stage6; /* shuffle_wn_stage6, len=512 */
  inputs_Task_ifft_0[35] = &extdata_shuffle_wn_stage7; /* shuffle_wn_stage7, len=512 */
  node_Task_ifft_0.numOutputs = 2U;
  node_Task_ifft_0.outputs = outputs_Task_ifft_0;
  outputs_Task_ifft_0[0] = &data_ifft_output_real_0_0;
  outputs_Task_ifft_0[1] = &data_ifft_output_imag_0_0;
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
  node_Task_ifft_1_1.computeDelayMs = 204800000U;
  node_Task_ifft_1_1.indegree = 0U;
  node_Task_ifft_1_1.mode = DAG_NODE_ASYNC;
  node_Task_ifft_1_1.numInputs = 36U;
  node_Task_ifft_1_1.inputs = inputs_Task_ifft_1_1;
  inputs_Task_ifft_1_1[0] = &extdata_cu_input_real_0_1; /* cu_input_real_0_1, len=256 */
  inputs_Task_ifft_1_1[1] = &extdata_cu_input_imag_0_1; /* cu_input_imag_0_1, len=256 */
  inputs_Task_ifft_1_1[2] = &extdata_zc_real_1; /* zc_real_1, len=256 */
  inputs_Task_ifft_1_1[3] = &extdata_zc_imag_1; /* zc_imag_1, len=256 */
  inputs_Task_ifft_1_1[4] = &extdata_cos_stage3; /* cos_stage3, len=256 */
  inputs_Task_ifft_1_1[5] = &extdata_cos_stage4; /* cos_stage4, len=256 */
  inputs_Task_ifft_1_1[6] = &extdata_cos_stage5; /* cos_stage5, len=256 */
  inputs_Task_ifft_1_1[7] = &extdata_cos_stage6; /* cos_stage6, len=256 */
  inputs_Task_ifft_1_1[8] = &extdata_cos_stage7; /* cos_stage7, len=256 */
  inputs_Task_ifft_1_1[9] = &extdata_cos_stage8; /* cos_stage8, len=256 */
  inputs_Task_ifft_1_1[10] = &extdata_cos_stage9; /* cos_stage9, len=256 */
  inputs_Task_ifft_1_1[11] = &extdata_cos_stage10; /* cos_stage10, len=256 */
  inputs_Task_ifft_1_1[12] = &extdata_sin_stage3; /* sin_stage3, len=256 */
  inputs_Task_ifft_1_1[13] = &extdata_sin_stage4; /* sin_stage4, len=256 */
  inputs_Task_ifft_1_1[14] = &extdata_sin_stage5; /* sin_stage5, len=256 */
  inputs_Task_ifft_1_1[15] = &extdata_sin_stage6; /* sin_stage6, len=256 */
  inputs_Task_ifft_1_1[16] = &extdata_sin_stage7; /* sin_stage7, len=256 */
  inputs_Task_ifft_1_1[17] = &extdata_sin_stage8; /* sin_stage8, len=256 */
  inputs_Task_ifft_1_1[18] = &extdata_sin_stage9; /* sin_stage9, len=256 */
  inputs_Task_ifft_1_1[19] = &extdata_sin_stage10; /* sin_stage10, len=256 */
  inputs_Task_ifft_1_1[20] = &extdata_shuffle_add_stage0; /* shuffle_add_stage0, len=512 */
  inputs_Task_ifft_1_1[21] = &extdata_shuffle_add_stage1; /* shuffle_add_stage1, len=512 */
  inputs_Task_ifft_1_1[22] = &extdata_shuffle_add_stage2; /* shuffle_add_stage2, len=512 */
  inputs_Task_ifft_1_1[23] = &extdata_shuffle_add_stage3; /* shuffle_add_stage3, len=512 */
  inputs_Task_ifft_1_1[24] = &extdata_shuffle_add_stage4; /* shuffle_add_stage4, len=512 */
  inputs_Task_ifft_1_1[25] = &extdata_shuffle_add_stage5; /* shuffle_add_stage5, len=512 */
  inputs_Task_ifft_1_1[26] = &extdata_shuffle_add_stage6; /* shuffle_add_stage6, len=512 */
  inputs_Task_ifft_1_1[27] = &extdata_shuffle_add_stage7; /* shuffle_add_stage7, len=512 */
  inputs_Task_ifft_1_1[28] = &extdata_shuffle_wn_stage0; /* shuffle_wn_stage0, len=512 */
  inputs_Task_ifft_1_1[29] = &extdata_shuffle_wn_stage1; /* shuffle_wn_stage1, len=512 */
  inputs_Task_ifft_1_1[30] = &extdata_shuffle_wn_stage2; /* shuffle_wn_stage2, len=512 */
  inputs_Task_ifft_1_1[31] = &extdata_shuffle_wn_stage3; /* shuffle_wn_stage3, len=512 */
  inputs_Task_ifft_1_1[32] = &extdata_shuffle_wn_stage4; /* shuffle_wn_stage4, len=512 */
  inputs_Task_ifft_1_1[33] = &extdata_shuffle_wn_stage5; /* shuffle_wn_stage5, len=512 */
  inputs_Task_ifft_1_1[34] = &extdata_shuffle_wn_stage6; /* shuffle_wn_stage6, len=512 */
  inputs_Task_ifft_1_1[35] = &extdata_shuffle_wn_stage7; /* shuffle_wn_stage7, len=512 */
  node_Task_ifft_1_1.numOutputs = 2U;
  node_Task_ifft_1_1.outputs = outputs_Task_ifft_1_1;
  outputs_Task_ifft_1_1[0] = &data_ifft_output_real_0_1;
  outputs_Task_ifft_1_1[1] = &data_ifft_output_imag_0_1;
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
  node_Task_ifft_2_2.computeDelayMs = 204800000U;
  node_Task_ifft_2_2.indegree = 0U;
  node_Task_ifft_2_2.mode = DAG_NODE_ASYNC;
  node_Task_ifft_2_2.numInputs = 36U;
  node_Task_ifft_2_2.inputs = inputs_Task_ifft_2_2;
  inputs_Task_ifft_2_2[0] = &extdata_cu_input_real_1_0; /* cu_input_real_1_0, len=256 */
  inputs_Task_ifft_2_2[1] = &extdata_cu_input_imag_1_0; /* cu_input_imag_1_0, len=256 */
  inputs_Task_ifft_2_2[2] = &extdata_zc_real_0; /* zc_real_0, len=256 */
  inputs_Task_ifft_2_2[3] = &extdata_zc_imag_0; /* zc_imag_0, len=256 */
  inputs_Task_ifft_2_2[4] = &extdata_cos_stage3; /* cos_stage3, len=256 */
  inputs_Task_ifft_2_2[5] = &extdata_cos_stage4; /* cos_stage4, len=256 */
  inputs_Task_ifft_2_2[6] = &extdata_cos_stage5; /* cos_stage5, len=256 */
  inputs_Task_ifft_2_2[7] = &extdata_cos_stage6; /* cos_stage6, len=256 */
  inputs_Task_ifft_2_2[8] = &extdata_cos_stage7; /* cos_stage7, len=256 */
  inputs_Task_ifft_2_2[9] = &extdata_cos_stage8; /* cos_stage8, len=256 */
  inputs_Task_ifft_2_2[10] = &extdata_cos_stage9; /* cos_stage9, len=256 */
  inputs_Task_ifft_2_2[11] = &extdata_cos_stage10; /* cos_stage10, len=256 */
  inputs_Task_ifft_2_2[12] = &extdata_sin_stage3; /* sin_stage3, len=256 */
  inputs_Task_ifft_2_2[13] = &extdata_sin_stage4; /* sin_stage4, len=256 */
  inputs_Task_ifft_2_2[14] = &extdata_sin_stage5; /* sin_stage5, len=256 */
  inputs_Task_ifft_2_2[15] = &extdata_sin_stage6; /* sin_stage6, len=256 */
  inputs_Task_ifft_2_2[16] = &extdata_sin_stage7; /* sin_stage7, len=256 */
  inputs_Task_ifft_2_2[17] = &extdata_sin_stage8; /* sin_stage8, len=256 */
  inputs_Task_ifft_2_2[18] = &extdata_sin_stage9; /* sin_stage9, len=256 */
  inputs_Task_ifft_2_2[19] = &extdata_sin_stage10; /* sin_stage10, len=256 */
  inputs_Task_ifft_2_2[20] = &extdata_shuffle_add_stage0; /* shuffle_add_stage0, len=512 */
  inputs_Task_ifft_2_2[21] = &extdata_shuffle_add_stage1; /* shuffle_add_stage1, len=512 */
  inputs_Task_ifft_2_2[22] = &extdata_shuffle_add_stage2; /* shuffle_add_stage2, len=512 */
  inputs_Task_ifft_2_2[23] = &extdata_shuffle_add_stage3; /* shuffle_add_stage3, len=512 */
  inputs_Task_ifft_2_2[24] = &extdata_shuffle_add_stage4; /* shuffle_add_stage4, len=512 */
  inputs_Task_ifft_2_2[25] = &extdata_shuffle_add_stage5; /* shuffle_add_stage5, len=512 */
  inputs_Task_ifft_2_2[26] = &extdata_shuffle_add_stage6; /* shuffle_add_stage6, len=512 */
  inputs_Task_ifft_2_2[27] = &extdata_shuffle_add_stage7; /* shuffle_add_stage7, len=512 */
  inputs_Task_ifft_2_2[28] = &extdata_shuffle_wn_stage0; /* shuffle_wn_stage0, len=512 */
  inputs_Task_ifft_2_2[29] = &extdata_shuffle_wn_stage1; /* shuffle_wn_stage1, len=512 */
  inputs_Task_ifft_2_2[30] = &extdata_shuffle_wn_stage2; /* shuffle_wn_stage2, len=512 */
  inputs_Task_ifft_2_2[31] = &extdata_shuffle_wn_stage3; /* shuffle_wn_stage3, len=512 */
  inputs_Task_ifft_2_2[32] = &extdata_shuffle_wn_stage4; /* shuffle_wn_stage4, len=512 */
  inputs_Task_ifft_2_2[33] = &extdata_shuffle_wn_stage5; /* shuffle_wn_stage5, len=512 */
  inputs_Task_ifft_2_2[34] = &extdata_shuffle_wn_stage6; /* shuffle_wn_stage6, len=512 */
  inputs_Task_ifft_2_2[35] = &extdata_shuffle_wn_stage7; /* shuffle_wn_stage7, len=512 */
  node_Task_ifft_2_2.numOutputs = 2U;
  node_Task_ifft_2_2.outputs = outputs_Task_ifft_2_2;
  outputs_Task_ifft_2_2[0] = &data_ifft_output_real_1_0;
  outputs_Task_ifft_2_2[1] = &data_ifft_output_imag_1_0;
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
  node_Task_ifft_3_3.computeDelayMs = 204800000U;
  node_Task_ifft_3_3.indegree = 0U;
  node_Task_ifft_3_3.mode = DAG_NODE_ASYNC;
  node_Task_ifft_3_3.numInputs = 36U;
  node_Task_ifft_3_3.inputs = inputs_Task_ifft_3_3;
  inputs_Task_ifft_3_3[0] = &extdata_cu_input_real_1_1; /* cu_input_real_1_1, len=256 */
  inputs_Task_ifft_3_3[1] = &extdata_cu_input_imag_1_1; /* cu_input_imag_1_1, len=256 */
  inputs_Task_ifft_3_3[2] = &extdata_zc_real_1; /* zc_real_1, len=256 */
  inputs_Task_ifft_3_3[3] = &extdata_zc_imag_1; /* zc_imag_1, len=256 */
  inputs_Task_ifft_3_3[4] = &extdata_cos_stage3; /* cos_stage3, len=256 */
  inputs_Task_ifft_3_3[5] = &extdata_cos_stage4; /* cos_stage4, len=256 */
  inputs_Task_ifft_3_3[6] = &extdata_cos_stage5; /* cos_stage5, len=256 */
  inputs_Task_ifft_3_3[7] = &extdata_cos_stage6; /* cos_stage6, len=256 */
  inputs_Task_ifft_3_3[8] = &extdata_cos_stage7; /* cos_stage7, len=256 */
  inputs_Task_ifft_3_3[9] = &extdata_cos_stage8; /* cos_stage8, len=256 */
  inputs_Task_ifft_3_3[10] = &extdata_cos_stage9; /* cos_stage9, len=256 */
  inputs_Task_ifft_3_3[11] = &extdata_cos_stage10; /* cos_stage10, len=256 */
  inputs_Task_ifft_3_3[12] = &extdata_sin_stage3; /* sin_stage3, len=256 */
  inputs_Task_ifft_3_3[13] = &extdata_sin_stage4; /* sin_stage4, len=256 */
  inputs_Task_ifft_3_3[14] = &extdata_sin_stage5; /* sin_stage5, len=256 */
  inputs_Task_ifft_3_3[15] = &extdata_sin_stage6; /* sin_stage6, len=256 */
  inputs_Task_ifft_3_3[16] = &extdata_sin_stage7; /* sin_stage7, len=256 */
  inputs_Task_ifft_3_3[17] = &extdata_sin_stage8; /* sin_stage8, len=256 */
  inputs_Task_ifft_3_3[18] = &extdata_sin_stage9; /* sin_stage9, len=256 */
  inputs_Task_ifft_3_3[19] = &extdata_sin_stage10; /* sin_stage10, len=256 */
  inputs_Task_ifft_3_3[20] = &extdata_shuffle_add_stage0; /* shuffle_add_stage0, len=512 */
  inputs_Task_ifft_3_3[21] = &extdata_shuffle_add_stage1; /* shuffle_add_stage1, len=512 */
  inputs_Task_ifft_3_3[22] = &extdata_shuffle_add_stage2; /* shuffle_add_stage2, len=512 */
  inputs_Task_ifft_3_3[23] = &extdata_shuffle_add_stage3; /* shuffle_add_stage3, len=512 */
  inputs_Task_ifft_3_3[24] = &extdata_shuffle_add_stage4; /* shuffle_add_stage4, len=512 */
  inputs_Task_ifft_3_3[25] = &extdata_shuffle_add_stage5; /* shuffle_add_stage5, len=512 */
  inputs_Task_ifft_3_3[26] = &extdata_shuffle_add_stage6; /* shuffle_add_stage6, len=512 */
  inputs_Task_ifft_3_3[27] = &extdata_shuffle_add_stage7; /* shuffle_add_stage7, len=512 */
  inputs_Task_ifft_3_3[28] = &extdata_shuffle_wn_stage0; /* shuffle_wn_stage0, len=512 */
  inputs_Task_ifft_3_3[29] = &extdata_shuffle_wn_stage1; /* shuffle_wn_stage1, len=512 */
  inputs_Task_ifft_3_3[30] = &extdata_shuffle_wn_stage2; /* shuffle_wn_stage2, len=512 */
  inputs_Task_ifft_3_3[31] = &extdata_shuffle_wn_stage3; /* shuffle_wn_stage3, len=512 */
  inputs_Task_ifft_3_3[32] = &extdata_shuffle_wn_stage4; /* shuffle_wn_stage4, len=512 */
  inputs_Task_ifft_3_3[33] = &extdata_shuffle_wn_stage5; /* shuffle_wn_stage5, len=512 */
  inputs_Task_ifft_3_3[34] = &extdata_shuffle_wn_stage6; /* shuffle_wn_stage6, len=512 */
  inputs_Task_ifft_3_3[35] = &extdata_shuffle_wn_stage7; /* shuffle_wn_stage7, len=512 */
  node_Task_ifft_3_3.numOutputs = 2U;
  node_Task_ifft_3_3.outputs = outputs_Task_ifft_3_3;
  outputs_Task_ifft_3_3[0] = &data_ifft_output_real_1_1;
  outputs_Task_ifft_3_3[1] = &data_ifft_output_imag_1_1;
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
  node_Task_ifft_4_4.computeDelayMs = 204800000U;
  node_Task_ifft_4_4.indegree = 0U;
  node_Task_ifft_4_4.mode = DAG_NODE_ASYNC;
  node_Task_ifft_4_4.numInputs = 36U;
  node_Task_ifft_4_4.inputs = inputs_Task_ifft_4_4;
  inputs_Task_ifft_4_4[0] = &extdata_cu_input_real_2_0; /* cu_input_real_2_0, len=256 */
  inputs_Task_ifft_4_4[1] = &extdata_cu_input_imag_2_0; /* cu_input_imag_2_0, len=256 */
  inputs_Task_ifft_4_4[2] = &extdata_zc_real_0; /* zc_real_0, len=256 */
  inputs_Task_ifft_4_4[3] = &extdata_zc_imag_0; /* zc_imag_0, len=256 */
  inputs_Task_ifft_4_4[4] = &extdata_cos_stage3; /* cos_stage3, len=256 */
  inputs_Task_ifft_4_4[5] = &extdata_cos_stage4; /* cos_stage4, len=256 */
  inputs_Task_ifft_4_4[6] = &extdata_cos_stage5; /* cos_stage5, len=256 */
  inputs_Task_ifft_4_4[7] = &extdata_cos_stage6; /* cos_stage6, len=256 */
  inputs_Task_ifft_4_4[8] = &extdata_cos_stage7; /* cos_stage7, len=256 */
  inputs_Task_ifft_4_4[9] = &extdata_cos_stage8; /* cos_stage8, len=256 */
  inputs_Task_ifft_4_4[10] = &extdata_cos_stage9; /* cos_stage9, len=256 */
  inputs_Task_ifft_4_4[11] = &extdata_cos_stage10; /* cos_stage10, len=256 */
  inputs_Task_ifft_4_4[12] = &extdata_sin_stage3; /* sin_stage3, len=256 */
  inputs_Task_ifft_4_4[13] = &extdata_sin_stage4; /* sin_stage4, len=256 */
  inputs_Task_ifft_4_4[14] = &extdata_sin_stage5; /* sin_stage5, len=256 */
  inputs_Task_ifft_4_4[15] = &extdata_sin_stage6; /* sin_stage6, len=256 */
  inputs_Task_ifft_4_4[16] = &extdata_sin_stage7; /* sin_stage7, len=256 */
  inputs_Task_ifft_4_4[17] = &extdata_sin_stage8; /* sin_stage8, len=256 */
  inputs_Task_ifft_4_4[18] = &extdata_sin_stage9; /* sin_stage9, len=256 */
  inputs_Task_ifft_4_4[19] = &extdata_sin_stage10; /* sin_stage10, len=256 */
  inputs_Task_ifft_4_4[20] = &extdata_shuffle_add_stage0; /* shuffle_add_stage0, len=512 */
  inputs_Task_ifft_4_4[21] = &extdata_shuffle_add_stage1; /* shuffle_add_stage1, len=512 */
  inputs_Task_ifft_4_4[22] = &extdata_shuffle_add_stage2; /* shuffle_add_stage2, len=512 */
  inputs_Task_ifft_4_4[23] = &extdata_shuffle_add_stage3; /* shuffle_add_stage3, len=512 */
  inputs_Task_ifft_4_4[24] = &extdata_shuffle_add_stage4; /* shuffle_add_stage4, len=512 */
  inputs_Task_ifft_4_4[25] = &extdata_shuffle_add_stage5; /* shuffle_add_stage5, len=512 */
  inputs_Task_ifft_4_4[26] = &extdata_shuffle_add_stage6; /* shuffle_add_stage6, len=512 */
  inputs_Task_ifft_4_4[27] = &extdata_shuffle_add_stage7; /* shuffle_add_stage7, len=512 */
  inputs_Task_ifft_4_4[28] = &extdata_shuffle_wn_stage0; /* shuffle_wn_stage0, len=512 */
  inputs_Task_ifft_4_4[29] = &extdata_shuffle_wn_stage1; /* shuffle_wn_stage1, len=512 */
  inputs_Task_ifft_4_4[30] = &extdata_shuffle_wn_stage2; /* shuffle_wn_stage2, len=512 */
  inputs_Task_ifft_4_4[31] = &extdata_shuffle_wn_stage3; /* shuffle_wn_stage3, len=512 */
  inputs_Task_ifft_4_4[32] = &extdata_shuffle_wn_stage4; /* shuffle_wn_stage4, len=512 */
  inputs_Task_ifft_4_4[33] = &extdata_shuffle_wn_stage5; /* shuffle_wn_stage5, len=512 */
  inputs_Task_ifft_4_4[34] = &extdata_shuffle_wn_stage6; /* shuffle_wn_stage6, len=512 */
  inputs_Task_ifft_4_4[35] = &extdata_shuffle_wn_stage7; /* shuffle_wn_stage7, len=512 */
  node_Task_ifft_4_4.numOutputs = 2U;
  node_Task_ifft_4_4.outputs = outputs_Task_ifft_4_4;
  outputs_Task_ifft_4_4[0] = &data_ifft_output_real_2_0;
  outputs_Task_ifft_4_4[1] = &data_ifft_output_imag_2_0;
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
  node_Task_ifft_5_5.computeDelayMs = 204800000U;
  node_Task_ifft_5_5.indegree = 0U;
  node_Task_ifft_5_5.mode = DAG_NODE_ASYNC;
  node_Task_ifft_5_5.numInputs = 36U;
  node_Task_ifft_5_5.inputs = inputs_Task_ifft_5_5;
  inputs_Task_ifft_5_5[0] = &extdata_cu_input_real_2_1; /* cu_input_real_2_1, len=256 */
  inputs_Task_ifft_5_5[1] = &extdata_cu_input_imag_2_1; /* cu_input_imag_2_1, len=256 */
  inputs_Task_ifft_5_5[2] = &extdata_zc_real_1; /* zc_real_1, len=256 */
  inputs_Task_ifft_5_5[3] = &extdata_zc_imag_1; /* zc_imag_1, len=256 */
  inputs_Task_ifft_5_5[4] = &extdata_cos_stage3; /* cos_stage3, len=256 */
  inputs_Task_ifft_5_5[5] = &extdata_cos_stage4; /* cos_stage4, len=256 */
  inputs_Task_ifft_5_5[6] = &extdata_cos_stage5; /* cos_stage5, len=256 */
  inputs_Task_ifft_5_5[7] = &extdata_cos_stage6; /* cos_stage6, len=256 */
  inputs_Task_ifft_5_5[8] = &extdata_cos_stage7; /* cos_stage7, len=256 */
  inputs_Task_ifft_5_5[9] = &extdata_cos_stage8; /* cos_stage8, len=256 */
  inputs_Task_ifft_5_5[10] = &extdata_cos_stage9; /* cos_stage9, len=256 */
  inputs_Task_ifft_5_5[11] = &extdata_cos_stage10; /* cos_stage10, len=256 */
  inputs_Task_ifft_5_5[12] = &extdata_sin_stage3; /* sin_stage3, len=256 */
  inputs_Task_ifft_5_5[13] = &extdata_sin_stage4; /* sin_stage4, len=256 */
  inputs_Task_ifft_5_5[14] = &extdata_sin_stage5; /* sin_stage5, len=256 */
  inputs_Task_ifft_5_5[15] = &extdata_sin_stage6; /* sin_stage6, len=256 */
  inputs_Task_ifft_5_5[16] = &extdata_sin_stage7; /* sin_stage7, len=256 */
  inputs_Task_ifft_5_5[17] = &extdata_sin_stage8; /* sin_stage8, len=256 */
  inputs_Task_ifft_5_5[18] = &extdata_sin_stage9; /* sin_stage9, len=256 */
  inputs_Task_ifft_5_5[19] = &extdata_sin_stage10; /* sin_stage10, len=256 */
  inputs_Task_ifft_5_5[20] = &extdata_shuffle_add_stage0; /* shuffle_add_stage0, len=512 */
  inputs_Task_ifft_5_5[21] = &extdata_shuffle_add_stage1; /* shuffle_add_stage1, len=512 */
  inputs_Task_ifft_5_5[22] = &extdata_shuffle_add_stage2; /* shuffle_add_stage2, len=512 */
  inputs_Task_ifft_5_5[23] = &extdata_shuffle_add_stage3; /* shuffle_add_stage3, len=512 */
  inputs_Task_ifft_5_5[24] = &extdata_shuffle_add_stage4; /* shuffle_add_stage4, len=512 */
  inputs_Task_ifft_5_5[25] = &extdata_shuffle_add_stage5; /* shuffle_add_stage5, len=512 */
  inputs_Task_ifft_5_5[26] = &extdata_shuffle_add_stage6; /* shuffle_add_stage6, len=512 */
  inputs_Task_ifft_5_5[27] = &extdata_shuffle_add_stage7; /* shuffle_add_stage7, len=512 */
  inputs_Task_ifft_5_5[28] = &extdata_shuffle_wn_stage0; /* shuffle_wn_stage0, len=512 */
  inputs_Task_ifft_5_5[29] = &extdata_shuffle_wn_stage1; /* shuffle_wn_stage1, len=512 */
  inputs_Task_ifft_5_5[30] = &extdata_shuffle_wn_stage2; /* shuffle_wn_stage2, len=512 */
  inputs_Task_ifft_5_5[31] = &extdata_shuffle_wn_stage3; /* shuffle_wn_stage3, len=512 */
  inputs_Task_ifft_5_5[32] = &extdata_shuffle_wn_stage4; /* shuffle_wn_stage4, len=512 */
  inputs_Task_ifft_5_5[33] = &extdata_shuffle_wn_stage5; /* shuffle_wn_stage5, len=512 */
  inputs_Task_ifft_5_5[34] = &extdata_shuffle_wn_stage6; /* shuffle_wn_stage6, len=512 */
  inputs_Task_ifft_5_5[35] = &extdata_shuffle_wn_stage7; /* shuffle_wn_stage7, len=512 */
  node_Task_ifft_5_5.numOutputs = 2U;
  node_Task_ifft_5_5.outputs = outputs_Task_ifft_5_5;
  outputs_Task_ifft_5_5[0] = &data_ifft_output_real_2_1;
  outputs_Task_ifft_5_5[1] = &data_ifft_output_imag_2_1;
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
  node_Task_fft_6.computeDelayMs = 192000000U;
  node_Task_fft_6.indegree = 1U;
  node_Task_fft_6.mode = DAG_NODE_ASYNC;
  node_Task_fft_6.numInputs = 34U;
  node_Task_fft_6.inputs = inputs_Task_fft_6;
  inputs_Task_fft_6[0] = &extdata_cos_stage3; /* cos_stage3, len=256 */
  inputs_Task_fft_6[1] = &extdata_cos_stage4; /* cos_stage4, len=256 */
  inputs_Task_fft_6[2] = &extdata_cos_stage5; /* cos_stage5, len=256 */
  inputs_Task_fft_6[3] = &extdata_cos_stage6; /* cos_stage6, len=256 */
  inputs_Task_fft_6[4] = &extdata_cos_stage7; /* cos_stage7, len=256 */
  inputs_Task_fft_6[5] = &extdata_cos_stage8; /* cos_stage8, len=256 */
  inputs_Task_fft_6[6] = &extdata_cos_stage9; /* cos_stage9, len=256 */
  inputs_Task_fft_6[7] = &extdata_cos_stage10; /* cos_stage10, len=256 */
  inputs_Task_fft_6[8] = &extdata_sin_stage3; /* sin_stage3, len=256 */
  inputs_Task_fft_6[9] = &extdata_sin_stage4; /* sin_stage4, len=256 */
  inputs_Task_fft_6[10] = &extdata_sin_stage5; /* sin_stage5, len=256 */
  inputs_Task_fft_6[11] = &extdata_sin_stage6; /* sin_stage6, len=256 */
  inputs_Task_fft_6[12] = &extdata_sin_stage7; /* sin_stage7, len=256 */
  inputs_Task_fft_6[13] = &extdata_sin_stage8; /* sin_stage8, len=256 */
  inputs_Task_fft_6[14] = &extdata_sin_stage9; /* sin_stage9, len=256 */
  inputs_Task_fft_6[15] = &extdata_sin_stage10; /* sin_stage10, len=256 */
  inputs_Task_fft_6[16] = &extdata_shuffle_add_stage0; /* shuffle_add_stage0, len=512 */
  inputs_Task_fft_6[17] = &extdata_shuffle_add_stage1; /* shuffle_add_stage1, len=512 */
  inputs_Task_fft_6[18] = &extdata_shuffle_add_stage2; /* shuffle_add_stage2, len=512 */
  inputs_Task_fft_6[19] = &extdata_shuffle_add_stage3; /* shuffle_add_stage3, len=512 */
  inputs_Task_fft_6[20] = &extdata_shuffle_add_stage4; /* shuffle_add_stage4, len=512 */
  inputs_Task_fft_6[21] = &extdata_shuffle_add_stage5; /* shuffle_add_stage5, len=512 */
  inputs_Task_fft_6[22] = &extdata_shuffle_add_stage6; /* shuffle_add_stage6, len=512 */
  inputs_Task_fft_6[23] = &extdata_shuffle_add_stage7; /* shuffle_add_stage7, len=512 */
  inputs_Task_fft_6[24] = &extdata_shuffle_wn_stage0; /* shuffle_wn_stage0, len=512 */
  inputs_Task_fft_6[25] = &extdata_shuffle_wn_stage1; /* shuffle_wn_stage1, len=512 */
  inputs_Task_fft_6[26] = &extdata_shuffle_wn_stage2; /* shuffle_wn_stage2, len=512 */
  inputs_Task_fft_6[27] = &extdata_shuffle_wn_stage3; /* shuffle_wn_stage3, len=512 */
  inputs_Task_fft_6[28] = &extdata_shuffle_wn_stage4; /* shuffle_wn_stage4, len=512 */
  inputs_Task_fft_6[29] = &extdata_shuffle_wn_stage5; /* shuffle_wn_stage5, len=512 */
  inputs_Task_fft_6[30] = &extdata_shuffle_wn_stage6; /* shuffle_wn_stage6, len=512 */
  inputs_Task_fft_6[31] = &extdata_shuffle_wn_stage7; /* shuffle_wn_stage7, len=512 */
  inputs_Task_fft_6[32] = &data_ifft_output_real_0_0; /* ifft_output_real_0_0, len=64 */
  inputs_Task_fft_6[33] = &data_ifft_output_imag_0_0; /* ifft_output_imag_0_0, len=64 */
  ctx_Task_fft_6.cu_id = CU_ANY_ID;
  ctx_Task_fft_6.job_id = 6U;
  ctx_Task_fft_6.node = &node_Task_fft_6;

  /* Node Task_fft*1 */
  memset(&node_Task_fft_1_7, 0, sizeof(DagNode));
  node_Task_fft_1_7.name = "Task_fft*1";
  node_Task_fft_1_7.run = vCuNodeWork;
  node_Task_fft_1_7.arg = &ctx_Task_fft_1_7;
  node_Task_fft_1_7.computeDelayMs = 192000000U;
  node_Task_fft_1_7.indegree = 1U;
  node_Task_fft_1_7.mode = DAG_NODE_ASYNC;
  node_Task_fft_1_7.numInputs = 34U;
  node_Task_fft_1_7.inputs = inputs_Task_fft_1_7;
  inputs_Task_fft_1_7[0] = &extdata_cos_stage3; /* cos_stage3, len=256 */
  inputs_Task_fft_1_7[1] = &extdata_cos_stage4; /* cos_stage4, len=256 */
  inputs_Task_fft_1_7[2] = &extdata_cos_stage5; /* cos_stage5, len=256 */
  inputs_Task_fft_1_7[3] = &extdata_cos_stage6; /* cos_stage6, len=256 */
  inputs_Task_fft_1_7[4] = &extdata_cos_stage7; /* cos_stage7, len=256 */
  inputs_Task_fft_1_7[5] = &extdata_cos_stage8; /* cos_stage8, len=256 */
  inputs_Task_fft_1_7[6] = &extdata_cos_stage9; /* cos_stage9, len=256 */
  inputs_Task_fft_1_7[7] = &extdata_cos_stage10; /* cos_stage10, len=256 */
  inputs_Task_fft_1_7[8] = &extdata_sin_stage3; /* sin_stage3, len=256 */
  inputs_Task_fft_1_7[9] = &extdata_sin_stage4; /* sin_stage4, len=256 */
  inputs_Task_fft_1_7[10] = &extdata_sin_stage5; /* sin_stage5, len=256 */
  inputs_Task_fft_1_7[11] = &extdata_sin_stage6; /* sin_stage6, len=256 */
  inputs_Task_fft_1_7[12] = &extdata_sin_stage7; /* sin_stage7, len=256 */
  inputs_Task_fft_1_7[13] = &extdata_sin_stage8; /* sin_stage8, len=256 */
  inputs_Task_fft_1_7[14] = &extdata_sin_stage9; /* sin_stage9, len=256 */
  inputs_Task_fft_1_7[15] = &extdata_sin_stage10; /* sin_stage10, len=256 */
  inputs_Task_fft_1_7[16] = &extdata_shuffle_add_stage0; /* shuffle_add_stage0, len=512 */
  inputs_Task_fft_1_7[17] = &extdata_shuffle_add_stage1; /* shuffle_add_stage1, len=512 */
  inputs_Task_fft_1_7[18] = &extdata_shuffle_add_stage2; /* shuffle_add_stage2, len=512 */
  inputs_Task_fft_1_7[19] = &extdata_shuffle_add_stage3; /* shuffle_add_stage3, len=512 */
  inputs_Task_fft_1_7[20] = &extdata_shuffle_add_stage4; /* shuffle_add_stage4, len=512 */
  inputs_Task_fft_1_7[21] = &extdata_shuffle_add_stage5; /* shuffle_add_stage5, len=512 */
  inputs_Task_fft_1_7[22] = &extdata_shuffle_add_stage6; /* shuffle_add_stage6, len=512 */
  inputs_Task_fft_1_7[23] = &extdata_shuffle_add_stage7; /* shuffle_add_stage7, len=512 */
  inputs_Task_fft_1_7[24] = &extdata_shuffle_wn_stage0; /* shuffle_wn_stage0, len=512 */
  inputs_Task_fft_1_7[25] = &extdata_shuffle_wn_stage1; /* shuffle_wn_stage1, len=512 */
  inputs_Task_fft_1_7[26] = &extdata_shuffle_wn_stage2; /* shuffle_wn_stage2, len=512 */
  inputs_Task_fft_1_7[27] = &extdata_shuffle_wn_stage3; /* shuffle_wn_stage3, len=512 */
  inputs_Task_fft_1_7[28] = &extdata_shuffle_wn_stage4; /* shuffle_wn_stage4, len=512 */
  inputs_Task_fft_1_7[29] = &extdata_shuffle_wn_stage5; /* shuffle_wn_stage5, len=512 */
  inputs_Task_fft_1_7[30] = &extdata_shuffle_wn_stage6; /* shuffle_wn_stage6, len=512 */
  inputs_Task_fft_1_7[31] = &extdata_shuffle_wn_stage7; /* shuffle_wn_stage7, len=512 */
  inputs_Task_fft_1_7[32] = &data_ifft_output_real_0_1; /* ifft_output_real_0_1, len=64 */
  inputs_Task_fft_1_7[33] = &data_ifft_output_imag_0_1; /* ifft_output_imag_0_1, len=64 */
  ctx_Task_fft_1_7.cu_id = CU_ANY_ID;
  ctx_Task_fft_1_7.job_id = 7U;
  ctx_Task_fft_1_7.node = &node_Task_fft_1_7;

  /* Node Task_fft*2 */
  memset(&node_Task_fft_2_8, 0, sizeof(DagNode));
  node_Task_fft_2_8.name = "Task_fft*2";
  node_Task_fft_2_8.run = vCuNodeWork;
  node_Task_fft_2_8.arg = &ctx_Task_fft_2_8;
  node_Task_fft_2_8.computeDelayMs = 192000000U;
  node_Task_fft_2_8.indegree = 1U;
  node_Task_fft_2_8.mode = DAG_NODE_ASYNC;
  node_Task_fft_2_8.numInputs = 34U;
  node_Task_fft_2_8.inputs = inputs_Task_fft_2_8;
  inputs_Task_fft_2_8[0] = &extdata_cos_stage3; /* cos_stage3, len=256 */
  inputs_Task_fft_2_8[1] = &extdata_cos_stage4; /* cos_stage4, len=256 */
  inputs_Task_fft_2_8[2] = &extdata_cos_stage5; /* cos_stage5, len=256 */
  inputs_Task_fft_2_8[3] = &extdata_cos_stage6; /* cos_stage6, len=256 */
  inputs_Task_fft_2_8[4] = &extdata_cos_stage7; /* cos_stage7, len=256 */
  inputs_Task_fft_2_8[5] = &extdata_cos_stage8; /* cos_stage8, len=256 */
  inputs_Task_fft_2_8[6] = &extdata_cos_stage9; /* cos_stage9, len=256 */
  inputs_Task_fft_2_8[7] = &extdata_cos_stage10; /* cos_stage10, len=256 */
  inputs_Task_fft_2_8[8] = &extdata_sin_stage3; /* sin_stage3, len=256 */
  inputs_Task_fft_2_8[9] = &extdata_sin_stage4; /* sin_stage4, len=256 */
  inputs_Task_fft_2_8[10] = &extdata_sin_stage5; /* sin_stage5, len=256 */
  inputs_Task_fft_2_8[11] = &extdata_sin_stage6; /* sin_stage6, len=256 */
  inputs_Task_fft_2_8[12] = &extdata_sin_stage7; /* sin_stage7, len=256 */
  inputs_Task_fft_2_8[13] = &extdata_sin_stage8; /* sin_stage8, len=256 */
  inputs_Task_fft_2_8[14] = &extdata_sin_stage9; /* sin_stage9, len=256 */
  inputs_Task_fft_2_8[15] = &extdata_sin_stage10; /* sin_stage10, len=256 */
  inputs_Task_fft_2_8[16] = &extdata_shuffle_add_stage0; /* shuffle_add_stage0, len=512 */
  inputs_Task_fft_2_8[17] = &extdata_shuffle_add_stage1; /* shuffle_add_stage1, len=512 */
  inputs_Task_fft_2_8[18] = &extdata_shuffle_add_stage2; /* shuffle_add_stage2, len=512 */
  inputs_Task_fft_2_8[19] = &extdata_shuffle_add_stage3; /* shuffle_add_stage3, len=512 */
  inputs_Task_fft_2_8[20] = &extdata_shuffle_add_stage4; /* shuffle_add_stage4, len=512 */
  inputs_Task_fft_2_8[21] = &extdata_shuffle_add_stage5; /* shuffle_add_stage5, len=512 */
  inputs_Task_fft_2_8[22] = &extdata_shuffle_add_stage6; /* shuffle_add_stage6, len=512 */
  inputs_Task_fft_2_8[23] = &extdata_shuffle_add_stage7; /* shuffle_add_stage7, len=512 */
  inputs_Task_fft_2_8[24] = &extdata_shuffle_wn_stage0; /* shuffle_wn_stage0, len=512 */
  inputs_Task_fft_2_8[25] = &extdata_shuffle_wn_stage1; /* shuffle_wn_stage1, len=512 */
  inputs_Task_fft_2_8[26] = &extdata_shuffle_wn_stage2; /* shuffle_wn_stage2, len=512 */
  inputs_Task_fft_2_8[27] = &extdata_shuffle_wn_stage3; /* shuffle_wn_stage3, len=512 */
  inputs_Task_fft_2_8[28] = &extdata_shuffle_wn_stage4; /* shuffle_wn_stage4, len=512 */
  inputs_Task_fft_2_8[29] = &extdata_shuffle_wn_stage5; /* shuffle_wn_stage5, len=512 */
  inputs_Task_fft_2_8[30] = &extdata_shuffle_wn_stage6; /* shuffle_wn_stage6, len=512 */
  inputs_Task_fft_2_8[31] = &extdata_shuffle_wn_stage7; /* shuffle_wn_stage7, len=512 */
  inputs_Task_fft_2_8[32] = &data_ifft_output_real_1_0; /* ifft_output_real_1_0, len=64 */
  inputs_Task_fft_2_8[33] = &data_ifft_output_imag_1_0; /* ifft_output_imag_1_0, len=64 */
  ctx_Task_fft_2_8.cu_id = CU_ANY_ID;
  ctx_Task_fft_2_8.job_id = 8U;
  ctx_Task_fft_2_8.node = &node_Task_fft_2_8;

  /* Node Task_fft*3 */
  memset(&node_Task_fft_3_9, 0, sizeof(DagNode));
  node_Task_fft_3_9.name = "Task_fft*3";
  node_Task_fft_3_9.run = vCuNodeWork;
  node_Task_fft_3_9.arg = &ctx_Task_fft_3_9;
  node_Task_fft_3_9.computeDelayMs = 192000000U;
  node_Task_fft_3_9.indegree = 1U;
  node_Task_fft_3_9.mode = DAG_NODE_ASYNC;
  node_Task_fft_3_9.numInputs = 34U;
  node_Task_fft_3_9.inputs = inputs_Task_fft_3_9;
  inputs_Task_fft_3_9[0] = &extdata_cos_stage3; /* cos_stage3, len=256 */
  inputs_Task_fft_3_9[1] = &extdata_cos_stage4; /* cos_stage4, len=256 */
  inputs_Task_fft_3_9[2] = &extdata_cos_stage5; /* cos_stage5, len=256 */
  inputs_Task_fft_3_9[3] = &extdata_cos_stage6; /* cos_stage6, len=256 */
  inputs_Task_fft_3_9[4] = &extdata_cos_stage7; /* cos_stage7, len=256 */
  inputs_Task_fft_3_9[5] = &extdata_cos_stage8; /* cos_stage8, len=256 */
  inputs_Task_fft_3_9[6] = &extdata_cos_stage9; /* cos_stage9, len=256 */
  inputs_Task_fft_3_9[7] = &extdata_cos_stage10; /* cos_stage10, len=256 */
  inputs_Task_fft_3_9[8] = &extdata_sin_stage3; /* sin_stage3, len=256 */
  inputs_Task_fft_3_9[9] = &extdata_sin_stage4; /* sin_stage4, len=256 */
  inputs_Task_fft_3_9[10] = &extdata_sin_stage5; /* sin_stage5, len=256 */
  inputs_Task_fft_3_9[11] = &extdata_sin_stage6; /* sin_stage6, len=256 */
  inputs_Task_fft_3_9[12] = &extdata_sin_stage7; /* sin_stage7, len=256 */
  inputs_Task_fft_3_9[13] = &extdata_sin_stage8; /* sin_stage8, len=256 */
  inputs_Task_fft_3_9[14] = &extdata_sin_stage9; /* sin_stage9, len=256 */
  inputs_Task_fft_3_9[15] = &extdata_sin_stage10; /* sin_stage10, len=256 */
  inputs_Task_fft_3_9[16] = &extdata_shuffle_add_stage0; /* shuffle_add_stage0, len=512 */
  inputs_Task_fft_3_9[17] = &extdata_shuffle_add_stage1; /* shuffle_add_stage1, len=512 */
  inputs_Task_fft_3_9[18] = &extdata_shuffle_add_stage2; /* shuffle_add_stage2, len=512 */
  inputs_Task_fft_3_9[19] = &extdata_shuffle_add_stage3; /* shuffle_add_stage3, len=512 */
  inputs_Task_fft_3_9[20] = &extdata_shuffle_add_stage4; /* shuffle_add_stage4, len=512 */
  inputs_Task_fft_3_9[21] = &extdata_shuffle_add_stage5; /* shuffle_add_stage5, len=512 */
  inputs_Task_fft_3_9[22] = &extdata_shuffle_add_stage6; /* shuffle_add_stage6, len=512 */
  inputs_Task_fft_3_9[23] = &extdata_shuffle_add_stage7; /* shuffle_add_stage7, len=512 */
  inputs_Task_fft_3_9[24] = &extdata_shuffle_wn_stage0; /* shuffle_wn_stage0, len=512 */
  inputs_Task_fft_3_9[25] = &extdata_shuffle_wn_stage1; /* shuffle_wn_stage1, len=512 */
  inputs_Task_fft_3_9[26] = &extdata_shuffle_wn_stage2; /* shuffle_wn_stage2, len=512 */
  inputs_Task_fft_3_9[27] = &extdata_shuffle_wn_stage3; /* shuffle_wn_stage3, len=512 */
  inputs_Task_fft_3_9[28] = &extdata_shuffle_wn_stage4; /* shuffle_wn_stage4, len=512 */
  inputs_Task_fft_3_9[29] = &extdata_shuffle_wn_stage5; /* shuffle_wn_stage5, len=512 */
  inputs_Task_fft_3_9[30] = &extdata_shuffle_wn_stage6; /* shuffle_wn_stage6, len=512 */
  inputs_Task_fft_3_9[31] = &extdata_shuffle_wn_stage7; /* shuffle_wn_stage7, len=512 */
  inputs_Task_fft_3_9[32] = &data_ifft_output_real_1_1; /* ifft_output_real_1_1, len=64 */
  inputs_Task_fft_3_9[33] = &data_ifft_output_imag_1_1; /* ifft_output_imag_1_1, len=64 */
  ctx_Task_fft_3_9.cu_id = CU_ANY_ID;
  ctx_Task_fft_3_9.job_id = 9U;
  ctx_Task_fft_3_9.node = &node_Task_fft_3_9;

  /* Node Task_fft*4 */
  memset(&node_Task_fft_4_10, 0, sizeof(DagNode));
  node_Task_fft_4_10.name = "Task_fft*4";
  node_Task_fft_4_10.run = vCuNodeWork;
  node_Task_fft_4_10.arg = &ctx_Task_fft_4_10;
  node_Task_fft_4_10.computeDelayMs = 192000000U;
  node_Task_fft_4_10.indegree = 1U;
  node_Task_fft_4_10.mode = DAG_NODE_ASYNC;
  node_Task_fft_4_10.numInputs = 34U;
  node_Task_fft_4_10.inputs = inputs_Task_fft_4_10;
  inputs_Task_fft_4_10[0] = &extdata_cos_stage3; /* cos_stage3, len=256 */
  inputs_Task_fft_4_10[1] = &extdata_cos_stage4; /* cos_stage4, len=256 */
  inputs_Task_fft_4_10[2] = &extdata_cos_stage5; /* cos_stage5, len=256 */
  inputs_Task_fft_4_10[3] = &extdata_cos_stage6; /* cos_stage6, len=256 */
  inputs_Task_fft_4_10[4] = &extdata_cos_stage7; /* cos_stage7, len=256 */
  inputs_Task_fft_4_10[5] = &extdata_cos_stage8; /* cos_stage8, len=256 */
  inputs_Task_fft_4_10[6] = &extdata_cos_stage9; /* cos_stage9, len=256 */
  inputs_Task_fft_4_10[7] = &extdata_cos_stage10; /* cos_stage10, len=256 */
  inputs_Task_fft_4_10[8] = &extdata_sin_stage3; /* sin_stage3, len=256 */
  inputs_Task_fft_4_10[9] = &extdata_sin_stage4; /* sin_stage4, len=256 */
  inputs_Task_fft_4_10[10] = &extdata_sin_stage5; /* sin_stage5, len=256 */
  inputs_Task_fft_4_10[11] = &extdata_sin_stage6; /* sin_stage6, len=256 */
  inputs_Task_fft_4_10[12] = &extdata_sin_stage7; /* sin_stage7, len=256 */
  inputs_Task_fft_4_10[13] = &extdata_sin_stage8; /* sin_stage8, len=256 */
  inputs_Task_fft_4_10[14] = &extdata_sin_stage9; /* sin_stage9, len=256 */
  inputs_Task_fft_4_10[15] = &extdata_sin_stage10; /* sin_stage10, len=256 */
  inputs_Task_fft_4_10[16] = &extdata_shuffle_add_stage0; /* shuffle_add_stage0, len=512 */
  inputs_Task_fft_4_10[17] = &extdata_shuffle_add_stage1; /* shuffle_add_stage1, len=512 */
  inputs_Task_fft_4_10[18] = &extdata_shuffle_add_stage2; /* shuffle_add_stage2, len=512 */
  inputs_Task_fft_4_10[19] = &extdata_shuffle_add_stage3; /* shuffle_add_stage3, len=512 */
  inputs_Task_fft_4_10[20] = &extdata_shuffle_add_stage4; /* shuffle_add_stage4, len=512 */
  inputs_Task_fft_4_10[21] = &extdata_shuffle_add_stage5; /* shuffle_add_stage5, len=512 */
  inputs_Task_fft_4_10[22] = &extdata_shuffle_add_stage6; /* shuffle_add_stage6, len=512 */
  inputs_Task_fft_4_10[23] = &extdata_shuffle_add_stage7; /* shuffle_add_stage7, len=512 */
  inputs_Task_fft_4_10[24] = &extdata_shuffle_wn_stage0; /* shuffle_wn_stage0, len=512 */
  inputs_Task_fft_4_10[25] = &extdata_shuffle_wn_stage1; /* shuffle_wn_stage1, len=512 */
  inputs_Task_fft_4_10[26] = &extdata_shuffle_wn_stage2; /* shuffle_wn_stage2, len=512 */
  inputs_Task_fft_4_10[27] = &extdata_shuffle_wn_stage3; /* shuffle_wn_stage3, len=512 */
  inputs_Task_fft_4_10[28] = &extdata_shuffle_wn_stage4; /* shuffle_wn_stage4, len=512 */
  inputs_Task_fft_4_10[29] = &extdata_shuffle_wn_stage5; /* shuffle_wn_stage5, len=512 */
  inputs_Task_fft_4_10[30] = &extdata_shuffle_wn_stage6; /* shuffle_wn_stage6, len=512 */
  inputs_Task_fft_4_10[31] = &extdata_shuffle_wn_stage7; /* shuffle_wn_stage7, len=512 */
  inputs_Task_fft_4_10[32] = &data_ifft_output_real_2_0; /* ifft_output_real_2_0, len=64 */
  inputs_Task_fft_4_10[33] = &data_ifft_output_imag_2_0; /* ifft_output_imag_2_0, len=64 */
  ctx_Task_fft_4_10.cu_id = CU_ANY_ID;
  ctx_Task_fft_4_10.job_id = 10U;
  ctx_Task_fft_4_10.node = &node_Task_fft_4_10;

  /* Node Task_fft*5 */
  memset(&node_Task_fft_5_11, 0, sizeof(DagNode));
  node_Task_fft_5_11.name = "Task_fft*5";
  node_Task_fft_5_11.run = vCuNodeWork;
  node_Task_fft_5_11.arg = &ctx_Task_fft_5_11;
  node_Task_fft_5_11.computeDelayMs = 192000000U;
  node_Task_fft_5_11.indegree = 1U;
  node_Task_fft_5_11.mode = DAG_NODE_ASYNC;
  node_Task_fft_5_11.numInputs = 34U;
  node_Task_fft_5_11.inputs = inputs_Task_fft_5_11;
  inputs_Task_fft_5_11[0] = &extdata_cos_stage3; /* cos_stage3, len=256 */
  inputs_Task_fft_5_11[1] = &extdata_cos_stage4; /* cos_stage4, len=256 */
  inputs_Task_fft_5_11[2] = &extdata_cos_stage5; /* cos_stage5, len=256 */
  inputs_Task_fft_5_11[3] = &extdata_cos_stage6; /* cos_stage6, len=256 */
  inputs_Task_fft_5_11[4] = &extdata_cos_stage7; /* cos_stage7, len=256 */
  inputs_Task_fft_5_11[5] = &extdata_cos_stage8; /* cos_stage8, len=256 */
  inputs_Task_fft_5_11[6] = &extdata_cos_stage9; /* cos_stage9, len=256 */
  inputs_Task_fft_5_11[7] = &extdata_cos_stage10; /* cos_stage10, len=256 */
  inputs_Task_fft_5_11[8] = &extdata_sin_stage3; /* sin_stage3, len=256 */
  inputs_Task_fft_5_11[9] = &extdata_sin_stage4; /* sin_stage4, len=256 */
  inputs_Task_fft_5_11[10] = &extdata_sin_stage5; /* sin_stage5, len=256 */
  inputs_Task_fft_5_11[11] = &extdata_sin_stage6; /* sin_stage6, len=256 */
  inputs_Task_fft_5_11[12] = &extdata_sin_stage7; /* sin_stage7, len=256 */
  inputs_Task_fft_5_11[13] = &extdata_sin_stage8; /* sin_stage8, len=256 */
  inputs_Task_fft_5_11[14] = &extdata_sin_stage9; /* sin_stage9, len=256 */
  inputs_Task_fft_5_11[15] = &extdata_sin_stage10; /* sin_stage10, len=256 */
  inputs_Task_fft_5_11[16] = &extdata_shuffle_add_stage0; /* shuffle_add_stage0, len=512 */
  inputs_Task_fft_5_11[17] = &extdata_shuffle_add_stage1; /* shuffle_add_stage1, len=512 */
  inputs_Task_fft_5_11[18] = &extdata_shuffle_add_stage2; /* shuffle_add_stage2, len=512 */
  inputs_Task_fft_5_11[19] = &extdata_shuffle_add_stage3; /* shuffle_add_stage3, len=512 */
  inputs_Task_fft_5_11[20] = &extdata_shuffle_add_stage4; /* shuffle_add_stage4, len=512 */
  inputs_Task_fft_5_11[21] = &extdata_shuffle_add_stage5; /* shuffle_add_stage5, len=512 */
  inputs_Task_fft_5_11[22] = &extdata_shuffle_add_stage6; /* shuffle_add_stage6, len=512 */
  inputs_Task_fft_5_11[23] = &extdata_shuffle_add_stage7; /* shuffle_add_stage7, len=512 */
  inputs_Task_fft_5_11[24] = &extdata_shuffle_wn_stage0; /* shuffle_wn_stage0, len=512 */
  inputs_Task_fft_5_11[25] = &extdata_shuffle_wn_stage1; /* shuffle_wn_stage1, len=512 */
  inputs_Task_fft_5_11[26] = &extdata_shuffle_wn_stage2; /* shuffle_wn_stage2, len=512 */
  inputs_Task_fft_5_11[27] = &extdata_shuffle_wn_stage3; /* shuffle_wn_stage3, len=512 */
  inputs_Task_fft_5_11[28] = &extdata_shuffle_wn_stage4; /* shuffle_wn_stage4, len=512 */
  inputs_Task_fft_5_11[29] = &extdata_shuffle_wn_stage5; /* shuffle_wn_stage5, len=512 */
  inputs_Task_fft_5_11[30] = &extdata_shuffle_wn_stage6; /* shuffle_wn_stage6, len=512 */
  inputs_Task_fft_5_11[31] = &extdata_shuffle_wn_stage7; /* shuffle_wn_stage7, len=512 */
  inputs_Task_fft_5_11[32] = &data_ifft_output_real_2_1; /* ifft_output_real_2_1, len=64 */
  inputs_Task_fft_5_11[33] = &data_ifft_output_imag_2_1; /* ifft_output_imag_2_1, len=64 */
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