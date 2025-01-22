#include "stage_data.h"

void configure_stage_data(StageData* stage, struct core* myCore)
{
    configure_pipeline_object(&(stage->inputState));
    configure_pipeline_object(&(stage->outputState));
    stage->myCore = myCore;
}
