#ifndef DECODE_STAGE_H
#define DECODE_STAGE_H

#include <stdint.h>
#include <stdbool.h>
#include "stage_data.h"

struct stageData;

/**
 * Decode Stage - Second stage in the pipeline
 * Responsible for instruction decoding and hazard detection
 */
typedef struct decodeStage
{
    struct stageData state;

    /**
     * @return Should stall?
     */
    bool (*doOperation)(struct decodeStage* self, Instruction execInst, Instruction memInst, Instruction wbInst);
} DecodeStage;

DecodeStage createDecodeStage();

#endif