#include "writeback_stage.h"

void updateRegisterValues(WriteBackStage* self)
{
    if (self->state.inputState.instruction.rdIdx == ZERO_REGISTER_INDEX ||
        self->state.inputState.instruction.rdIdx == IMMEDIATE_REGISTER_INDEX)
        return;
    
    if (self->state.inputState.instruction.opcode == Add ||
        self->state.inputState.instruction.opcode == Sub ||
        self->state.inputState.instruction.opcode == And ||
        self->state.inputState.instruction.opcode == Or  ||
        self->state.inputState.instruction.opcode == Xor ||
        self->state.inputState.instruction.opcode == Mul ||
        self->state.inputState.instruction.opcode == Sll ||
        self->state.inputState.instruction.opcode == Sra ||
        self->state.inputState.instruction.opcode == Srl)
    {
        self->state.myCore->registers[self->state.inputState.instruction.rdIdx].updated = 
            self->state.inputState.aluOperationOutput;
    }
    else if (self->state.inputState.instruction.opcode == Lw)
    {
        self->state.myCore->registers[self->state.inputState.instruction.rdIdx].updated = 
            self->state.inputState.memoryRetrieved;
    }
}

bool doWriteBackOperation(WriteBackStage* self)
{
    // First copy the output state from the input state.
    // Later update the output state with operation needed to be
    // done at this round
    self->state.outputState = self->state.inputState;

    updateRegisterValues(self);

    return false;       // write back never stalls
}

WriteBackStage createWriteBackStage() // xxx is the decode a mistake?
{
    WriteBackStage stage;
    stage.doOperation = doWriteBackOperation;

    return stage;
}