#ifndef EXECUTE_STAGE_H
#define EXECUTE_STAGE_H

#include <stdint.h>
#include <stdbool.h>
#include "stage_data.h"

struct stageData;

/**
 * Execute Stage - Third stage in the pipeline
 * Responsible for performing ALU operations
 */
typedef struct executeStage
{
    struct stageData state;
} ExecuteStage;

void configure_execute_stage(ExecuteStage* stage, struct core* myCore);

/**
 * @return Should stall?
 */
bool do_execute_operation(ExecuteStage* self);

#endif