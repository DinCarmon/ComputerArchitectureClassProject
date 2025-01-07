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
    StageData state;

    /**
     * @return Should stall?
     */
    bool (*doOperation)(struct fetchStage* self);
} FetchStage;

FetchStage createFetchStage();

#endif