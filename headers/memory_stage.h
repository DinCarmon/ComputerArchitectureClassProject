#ifndef MEMORY_STAGE_H
#define MEMORY_STAGE_H

#include <stdint.h>
#include <stdbool.h>
#include "stage_data.h"

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
    bool (*doOperation)(struct memoryStage* self);
} MemoryStage;

MemoryStage createMemoryStage();

#endif