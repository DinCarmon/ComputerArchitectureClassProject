#ifndef STAGE_DATA_H
#define STAGE_DATA_H

#include "pipeline_object.h"

struct core;                            // Shall be implemented later

/**
 * StageData - Contains all data needed for a pipeline stage
 */
typedef struct stageData {
    struct core* myCore;                // Core identifier
    PipelineObjectState inputState;     // Input state to the stage
    PipelineObjectState outputState;    // Output state from the stage

} StageData;

#endif