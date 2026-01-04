/* dag_debug.h - DAG debug MMIO helpers and config codes */
#ifndef DAG_DEBUG_H
#define DAG_DEBUG_H

#include <stdint.h>

/* DAG Debug 寄存器定义 */
#define DAG_DEBUG_BASE    0x20100000u
#define DAG_DEBUG_CU_ID   (DAG_DEBUG_BASE + 0x00u)  /* uint32_t cu_id_reg */
#define DAG_DEBUG_JOB_ID  (DAG_DEBUG_BASE + 0x04u)  /* uint32_t job_id_reg */
#define DAG_DEBUG_CONFIG  (DAG_DEBUG_BASE + 0x08u)  /* uint8_t config */

static inline void dag_debug_write32(uint32_t addr, uint32_t val) {
  *(volatile uint32_t*)addr = val;
}

static inline void dag_debug_write8(uint32_t addr, uint8_t val) {
  *(volatile uint8_t*)addr = val;
}

/* Suggested config codes (chronological order):
 * 0:  vDagSubmitReadyNode (Node queued)任务进入就绪队列
 * 1:  vDagWorkerTask (Worker dequeue/Node run start)Worker 线程开始执行任务
 * 2:  vDagPrepareAsyncOutputs (Async output buffer allocation)为异步/CU 任务准备输出缓存
 * 3:  vCuSubmitJob (High-level CU job submission start)高层 CU 作业提交开始
 * 4:  vCuHwStartJob (Hardware preparation start)硬件启动前的准备工作
 * 5:  vCuHwStartJob (Input DMA transfer start)开始通过 DMA 搬移输入数据到 CU
 * 6:  vCuHwStartJob (CU compute start - write config)写入 CU 配置寄存器，硬件开始计算
 * 7:  xPortHandleExternalInterrupt (PLIC Claim/ISR start)PLIC 响应中断，进入 ISR
 * 8:  vCuHandleIsr (Output DMA transfer start)ISR 开始通过 DMA 回收输出数据
 * 9:  vCuHandleIsr (Output DMA transfer end)ISR 完成数据回收
 * 10: xPortHandleExternalInterrupt (ISR end)ISR 处理完毕，退出中断
 * 11: vDagAsyncCompleteTask (Async completion dequeued)异步完成任务从队列中取出
 * 12: vDagFinalizeNode (Node finalization: produce/release/enqueue)节点最终化（释放输入、触发后继）
 * 13: vDagWorkerTask (Node run end - for sync nodes or after submission)Worker 线程完成当前任务处理
 */

#endif /* DAG_DEBUG_H */
