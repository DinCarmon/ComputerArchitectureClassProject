#ifndef EXECUTE_STAGE_H
#define EXECUTE_STAGE_H

#include <stdint.h>
#include <stdbool.h>
#include "stage_data.h"

/**
 * Execute Stage - Third stage in the pipeline
 * Responsible for performing ALU operations
 */
typedef struct executeStage
{
    StageData state;

    /**
     * @return Should stall?
     */
    bool (*doOperation)(struct executeStage* self);
} ExecuteStage;

ExecuteStage createExecuteStage();

#endif