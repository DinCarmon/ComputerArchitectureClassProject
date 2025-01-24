#include <stdio.h>

#include "fetch_stage.h"
#include "instruction.h"
#include "core.h"

bool do_fetch_operation(FetchStage* self)
{
    uint32_t instructionCodex = (self->state.myCore->instruction_memory[self->state.myCore->pc_register.now]);
    Instruction inst = create_instruction(instructionCodex);

    self->state.outputState.instruction = inst;
    self->state.outputState.instructionAddress = self->state.myCore->pc_register.now;

    // If it not a halt / the first time we reach a halt.
    // To check if it the first time a halt is reach we compare the address getting in the fetch stage and decode stage.
    // However there is an edge case where the first command is a halt
    if ((inst.opcode != Halt) ||
        (inst.opcode == Halt &&
         self->state.myCore->pc_register.now != self->state.myCore->decode_stage.state.inputState.instructionAddress) ||
        (inst.opcode == Halt && *self->state.myCore->p_cycle == 1))
        self->state.myCore->num_of_instructions_executed += 1;

    // If the instruction fetched is the halt instruction stop fetching advancing instructions.
    // Let the pc register stay in halt.
    if (inst.opcode != Halt)
    {
        if (self->state.myCore->pc_registered_updated_by != 1) // i.e the decode stage does not wish to change the pc register / did not run at this cycle yet
            self->state.myCore->pc_register.updated = self->state.myCore->pc_register.now + 1;
    }
    else
    {
        self->state.myCore->pc_register.updated = self->state.myCore->pc_register.now;
    }
    
    self->state.outputState.is_ready = true;
    return false;
}

void configure_fetch_stage(FetchStage* stage, struct core* myCore)
{
    configure_stage_data(&(stage->state), myCore);
}