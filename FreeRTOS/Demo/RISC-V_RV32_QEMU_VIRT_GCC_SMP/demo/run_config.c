#include "run_config.h"

UBaseType_t gDagWorkerCountOverride = 0u;

void vDagSetWorkerCountOverride(UBaseType_t wc) {
  taskENTER_CRITICAL();
  gDagWorkerCountOverride = wc;
  taskEXIT_CRITICAL();
}
