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
} DecodeStage;

void configure_decode_stage(DecodeStage* stage, struct core* myCore);

/**
 * @return Should stall?
 */
bool do_decode_operation(DecodeStage* self);

#endif