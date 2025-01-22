#ifndef PIPELINE_OBJECT_H
#define PIPELINE_OBJECT_H

#include <stdint.h>
#include "instruction.h"

/**
 * PipelineObjectState - Represents the state of an instruction in the pipeline
 * Contains all data needed to be passed between pipeline stages.
 * Not all parameters shall have a meaning.
 * For example, alu operations shall have no use of the memoryRetrieved field.
 */
typedef struct pipelineObjectState {
    uint32_t instructionAddress;        // Original address of instruction
    Instruction instruction;     // The instruction itself
    uint32_t rsValue;                   // Value of rs register
    uint32_t rtValue;                   // Value of rt register
    uint32_t rdValue;                   // Value of rd register
    uint32_t aluOperationOutput;        // Result from ALU
    uint32_t memoryRetrieved;           // Data retrieved from memory
} PipelineObjectState;

void configure_pipeline_object(PipelineObjectState* pipeline_object);

#endif