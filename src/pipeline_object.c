#include "pipeline_object.h"

void reset_pipeline_object(PipelineObjectState* pipeline_object)
{
    pipeline_object->aluOperationOutput = 0;
    pipeline_object->instructionAddress = 0;
    pipeline_object->memoryRetrieved = 0;
    pipeline_object->rdValue = 0;
    pipeline_object->rsValue = 0;
    pipeline_object->rtValue = 0;

    reset_instruction(&(pipeline_object->instruction));
}