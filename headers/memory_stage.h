#ifndef MEMORY_STAGE_H
#define MEMORY_STAGE_H

#include <stdint.h>
#include <stdbool.h>
#include "stage_data.h"
#include "cache_block.h"

/**
 * Memory Stage - Fourth stage in the pipeline
 * Handles memory operations and cache interactions
 */
typedef struct memoryStage
{
    struct stageData state;
    uint64_t num_of_cycles_on_same_command; // How many stalls we already went through
} MemoryStage;

void configure_memory_stage(MemoryStage* stage, struct core* myCore);

/**
 * @return Should stall?
 */
bool do_memory_operation(MemoryStage* self);

#endif