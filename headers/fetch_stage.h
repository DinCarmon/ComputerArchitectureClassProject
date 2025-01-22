#ifndef FETCH_STAGE_H
#define FETCH_STAGE_H

#include <stdint.h>
#include <stdbool.h>
#include "stage_data.h"

/**
 * Fetch Stage - First stage in the pipeline
 * Responsible for fetching instructions from memory
 */
typedef struct fetchStage
{
    struct stageData state;
} FetchStage;

void configure_fetch_stage(FetchStage* stage, struct core* myCore);

/**
 * @return Should stall?
 */
bool do_fetch_operation(FetchStage* self);

#endif