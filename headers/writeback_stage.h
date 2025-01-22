#ifndef WRITE_BACK_STAGE_H
#define WRITE_BACK_STAGE_H

#include <stdint.h>
#include <stdbool.h>
#include "stage_data.h"

/**
 * WriteBack Stage - Final stage in the pipeline
 * Responsible for writing results back to registers
 */
typedef struct writeBackStage
{
    struct stageData state;
} WriteBackStage;

void configure_writeback_stage(WriteBackStage* stage, struct core* myCore);

/**
 * @return Should stall?
 */
bool do_write_back_operation(WriteBackStage* self);

#endif