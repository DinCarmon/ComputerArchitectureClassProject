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
    StageData state;

    /**
     * @return Should stall?
     */
    bool (*doOperation)(struct writeBackStage* self);
} WriteBackStage;

WriteBackStage createDecodeStage();

#endif