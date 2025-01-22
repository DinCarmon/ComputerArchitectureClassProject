#include "fetch_stage.h"
#include "instruction.h"
#include "core.h"

bool doFetchOperation(FetchStage* self)
{
    uint32_t instructionCodex = (self->state.myCore->InstructionMemory[self->state.myCore->pc_register.now]);
    Instruction inst = createInstruction(instructionCodex);

    self->state.outputState.instruction = inst;
    self->state.outputState.instructionAddress = self->state.myCore->pc_register.now;

    // If the instruction fetched is the halt instruction stop fetching advancing instructions.
    // Let the pc register stay in halt.
    if (inst.opcode != Halt)
        self->state.myCore->pc_register.updated = self->state.myCore->pc_register.now + 1;
    else
        self->state.myCore->pc_register.updated = self->state.myCore->pc_register.now;
    
    return false;
}

FetchStage createFetchStage()
{
    FetchStage stage;
    stage.doOperation = doFetchOperation;

    return stage;
}