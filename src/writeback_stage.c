#include "writeback_stage.h"
#include "core.h"

void updateRegisterValues_writeback(WriteBackStage* self)
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

bool do_write_back_operation(WriteBackStage* self)
{
    // First copy the output state from the input state.
    // Later update the output state with operation needed to be
    // done at this round
    self->state.outputState = self->state.inputState;

	updateRegisterValues_writeback(self);

    if (self->state.inputState.instruction.opcode == Halt)
    {
        self->state.myCore->halt_cycle = *(self->state.myCore->p_cycle);
    }

    return false;       // write back never stalls
}

void configure_writeback_stage(WriteBackStage* stage, struct core* myCore)
{
    configure_stage_data(&(stage->state), myCore);
}