/* cu_offload.h */

#ifndef CU_OFFLOAD_H
#define CU_OFFLOAD_H

#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"

struct DagNode;

#define CU_MAX_COUNT  4  /* 现在只用 4 个 CU，将来可以扩展 */

typedef struct {
    TaskHandle_t waiter;
    struct DagNode *dag_node;
    uint32_t     job_id;
} CuSlot_t;

void vCuInit(void);

/* DAG 任务同步阻塞 offload ：提交一个 CU 任务并等待 PLIC IRQ 唤醒 */
void vCuSubmitJobAndWait(uint32_t cu_id, uint32_t job_id);

/* DAG 异步 offload ：提交 CU 任务，完成由 PLIC ISR 通知 DAG runtime */
void vCuSubmitDagJob(uint32_t cu_id, uint32_t job_id, struct DagNode *node);

/* 中断上下文：在 PLIC IRQ handler 里调用 */
void vCuHandleIsr(uint32_t cu_id,
                  BaseType_t *pxHigherPriorityTaskWoken);

/* TODO:写 CU / DMA 寄存器并启动运算 */
void vCuHwStartJob(uint32_t cu_id, uint32_t job_id);

#endif
