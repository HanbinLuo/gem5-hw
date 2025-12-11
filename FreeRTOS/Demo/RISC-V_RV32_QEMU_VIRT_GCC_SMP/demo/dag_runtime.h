/* dag_runtime.h
 *
 * FreeRTOS 上的一个极简 DAG 调度器：
 *  - DAG 节点可分为同步/异步两类
 *  - 异步节点 run() 返回后释放 worker，实际完成由 ISR 通知
 *  - DAG runtime 统一维护 ready 队列和 async-done 队列
 */

#ifndef DAG_RUNTIME_H
#define DAG_RUNTIME_H

#include <stdint.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

typedef enum {
    DAG_NODE_SYNC = 0,
    DAG_NODE_ASYNC = 1
} DagNodeMode;

typedef struct DagNode {
    const char *name;
    void (*run)(void *arg);
    void *arg;

    uint32_t indegree;
    uint32_t numSuccessors;
    struct DagNode **successors;

    DagNodeMode mode;
    volatile uint32_t async_pending;
} DagNode;

typedef struct {
    UBaseType_t workerCount;
    UBaseType_t workerPriority;
    UBaseType_t workerStackWords;
    UBaseType_t asyncPriority;
    UBaseType_t asyncStackWords;
    UBaseType_t readyQueueLen;
    UBaseType_t asyncDoneQueueLen;
    BaseType_t  useCoreAffinity;
} DagRuntimeConfig;

void vDagRuntimeStart(const DagRuntimeConfig *cfg);
void vDagSubmitReadyNode(DagNode *node);
void vDagNotifyAsyncDoneFromISR(DagNode *node,
                                BaseType_t *pxHigherPriorityTaskWoken);

#endif /* DAG_RUNTIME_H */
