#ifndef RUN_CONFIG_H
#define RUN_CONFIG_H

#include <stdint.h>
#include "FreeRTOS.h"

/* Override worker count (0 = use default NUM_WORKERS) */
extern UBaseType_t gDagWorkerCountOverride;
void vDagSetWorkerCountOverride(UBaseType_t wc);

/* Limit available CUs at runtime (0 = no limit) */
extern uint32_t gCuAvailableCount;
void vCuSetAvailableCount(uint32_t count);

#endif
