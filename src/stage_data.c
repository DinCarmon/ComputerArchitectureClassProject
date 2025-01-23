#include "stage_data.h"

void configure_stage_data(StageData* stage, struct core* myCore)
{
    reset_pipeline_object(&(stage->inputState));
    reset_pipeline_object(&(stage->outputState));
    stage->myCore = myCore;
}
