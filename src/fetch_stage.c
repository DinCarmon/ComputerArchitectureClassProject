#include "fetch_stage.h"

bool doFetchOperation(FetchStage* self)
{
    uint32_t instructionCodex = (self->state.myCore->instructionMemory[self->state.myCore->pcRegister.now]).now;
    Instruction inst = createInstruction(instructionCodex);

    self->state.outputState.instruction = inst;
    self->state.outputState.instructionAddress = self->state.myCore->pcRegister.now;

    // If the instruction fetched is the halt instruction stop fetching advancing instructions.
    // Let the pc register stay in halt.
    if (inst.opcode != Halt)
        self->state.myCore->pcRegister.updated = self->state.myCore->pcRegister.now + 1;
    else
        self->state.myCore->pcRegister.updated = self->state.myCore->pcRegister.now;
    
    return false;
}

FetchStage createFetchStage()
{
    FetchStage stage;
    stage.doOperation = doFetchOperation;

    return stage;
}