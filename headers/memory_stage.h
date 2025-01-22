#ifndef MEMORY_STAGE_H
#define MEMORY_STAGE_H

#include <stdint.h>
#include <stdbool.h>
#include "stage_data.h"
#include "cache_block.h"
#include "bus_manager.h"

/**
 * Memory Stage - Fourth stage in the pipeline
 * Handles memory operations and cache interactions
 */
typedef struct memoryStage
{
    StageData state;

    /**
     * @return Should stall?
     */
    bool (*doOperation)(struct memoryStage* self, BusManager* manager);
} MemoryStage;

MemoryStage createMemoryStage();

#endif