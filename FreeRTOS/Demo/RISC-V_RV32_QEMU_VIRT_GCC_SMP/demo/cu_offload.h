/* cu_offload.h */

#ifndef CU_OFFLOAD_H
#define CU_OFFLOAD_H

#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"

struct DagNode;

#define CU_MAX_COUNT 256        /* 现在只用 256 个 CU，将来可以扩展 */
#define CU_MAX_IO 4           /* 每个 CU 任务最多传 4 路输入/输出描述 */
#define CU_ANY_ID 0xFFFFFFFFu /* 让运行时选择空闲 CU */

/* CU 基址：支持按 cu_id 偏移（stride）映射多个 CU，间隔为 0x40 */
#define CU_BASE0 0x10100000UL
#define CU_STRIDE 0x100000UL

/* CU 寄存器偏移（地址规划占位，具体映射待硬件/文档确认） */
/* ComputeUnit register map (all offsets are byte addresses)
        0x00..0x03 : CU_ID    (32-bit)
        0x04..0x07 : JOB_ID   (32-bit)
        0x08..0x0B : SIZE     (32-bit)
        0x0C..0x0F : DELAY    (32-bit, ticks)
        0x10        : CONFIG   (write to start, 8-bit)
        0x11        : STATUS   (read, bit0 = done)
        0x12        : BUSY     (read, bit0 = busy)
*/

#define CU_ID_OFF 0x00
#define CU_JOB_OFF 0x04
#define CU_SIZE_OFF 0x08
#define CU_DELAY_OFF 0x0C
#define CU_CONFIG_OFF 0x10
#define CU_STATUS_OFF 0x11
#define CU_BUSY_OFF 0x12
#define CU_INPUT_ADDR0_OFF 0x1004
#define CU_INPUT_SIZE0_OFF 0x1000
#define CU_INPUT_STRIDE 0x800
#define CU_OUTPUT_ADDR0_OFF 0x3004
#define CU_OUTPUT_SIZE0_OFF 0x3000
#define CU_OUTPUT_STRIDE 0x800

static inline uintptr_t cu_base(uint32_t id) {
  return (uintptr_t)(CU_BASE0 + id * CU_STRIDE);
}

// 简单的 MMIO 读写（8-bit）
static inline void mmio_write8(uintptr_t addr, uint8_t v) {
  volatile uint8_t* p = (volatile uint8_t*)addr;
  *p = v;
}

static inline uint8_t mmio_read8(uintptr_t addr) {
  volatile uint8_t* p = (volatile uint8_t*)addr;
  return *p;
}

// 简单的 MMIO 读写（32-bit）
static inline void mmio_write32(uintptr_t addr, uint32_t v) {
  volatile uint32_t* p = (volatile uint32_t*)addr;
  *p = v;
}

static inline uint32_t mmio_read32(uintptr_t addr) {
  volatile uint32_t* p = (volatile uint32_t*)addr;
  return *p;
}

typedef struct {
  TaskHandle_t waiter;
  struct DagNode* dag_node;
  uint32_t job_id;
  struct {
    uint32_t numInputs;
    uint32_t numOutputs;
    uintptr_t inputAddrs[CU_MAX_IO];
    uint32_t inputSizes[CU_MAX_IO];
    uintptr_t outputAddrs[CU_MAX_IO];
    uint32_t outputSizes[CU_MAX_IO];
    uint32_t computeDelayMs;
    uint32_t storageSizeBytes;
  } job;
} CuSlot_t;

void vCuInit(void);

/* DAG 任务同步阻塞 offload ：提交一个 CU 任务并等待 PLIC IRQ 唤醒 */
uint32_t vCuSubmitJobAndWait(uint32_t cu_id, uint32_t job_id);

/* DAG 异步 offload ：提交 CU 任务，完成由 PLIC ISR 通知 DAG runtime */
uint32_t vCuSubmitDagJob(uint32_t cu_id, uint32_t job_id, struct DagNode* node);

/* 中断上下文：在 PLIC IRQ handler 里调用 */
void vCuHandleIsr(uint32_t cu_id, BaseType_t* pxHigherPriorityTaskWoken);

/* 写 CU / DMA 寄存器并启动运算 */
void vCuHwStartJob(uint32_t cu_id, uint32_t job_id, const CuSlot_t* job_info);

#endif
