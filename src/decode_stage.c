#include "decode_stage.h"
#include "core.h"

void updateRegisterValues_decode(DecodeStage* self)
{
    Instruction inst = self->state.inputState.instruction;

    if (inst.rsIdx == ZERO_REGISTER_INDEX)
        self->state.outputState.rsValue = 0;
    else if (inst.rsIdx == IMMEDIATE_REGISTER_INDEX)
        self->state.outputState.rsValue = inst.immediate;
    else
        self->state.outputState.rsValue = self->state.myCore->registers[inst.rsIdx].now;

    if (inst.rtIdx == ZERO_REGISTER_INDEX)
        self->state.outputState.rtValue = 0;
    else if (inst.rtIdx == IMMEDIATE_REGISTER_INDEX)
        self->state.outputState.rtValue = inst.immediate;
    else
        self->state.outputState.rtValue = self->state.myCore->registers[inst.rtIdx].now;

    if (inst.rdIdx == ZERO_REGISTER_INDEX)
        self->state.outputState.rdValue = 0;
    else if (inst.rdIdx == IMMEDIATE_REGISTER_INDEX)
        self->state.outputState.rdValue = inst.immediate;
    else
        self->state.outputState.rdValue = self->state.myCore->registers[inst.rdIdx].now;
}

bool check_RAW_Hazard(Instruction instNow, Instruction unfinishedInst)
{
    // current Rs RAW hazards:
    if (instNow.rsIdx == unfinishedInst.rdIdx   &&
        instNow.rsIdx != ZERO_REGISTER_INDEX    &&
        instNow.rsIdx != IMMEDIATE_REGISTER_INDEX)
    {
        // These opcodes do not need rs, and therefore should not cause a stall
        if (instNow.opcode != Jal && instNow.opcode != Halt)
        {
            // Rd is updated only in these instructions
            if (unfinishedInst.opcode == Add ||
                unfinishedInst.opcode == Sub ||
                unfinishedInst.opcode == And ||
                unfinishedInst.opcode == Or  ||
                unfinishedInst.opcode == Xor ||
                unfinishedInst.opcode == Mul ||
                unfinishedInst.opcode == Sll ||
                unfinishedInst.opcode == Sra ||
                unfinishedInst.opcode == Srl ||
                unfinishedInst.opcode == Lw)
                    return true;
        }
    }

    // Current Rt RAW hazards
    if (instNow.rtIdx == unfinishedInst.rdIdx   &&
        instNow.rtIdx != ZERO_REGISTER_INDEX    &&
        instNow.rtIdx != IMMEDIATE_REGISTER_INDEX)
    {
        // These opcodes do not need rt, and therefore should not cause a stall
        if (instNow.opcode != Jal && instNow.opcode != Halt)
        {
            // Rd is updated only in these instructions
            if (unfinishedInst.opcode == Add ||
                unfinishedInst.opcode == Sub ||
                unfinishedInst.opcode == And ||
                unfinishedInst.opcode == Or  ||
                unfinishedInst.opcode == Xor ||
                unfinishedInst.opcode == Mul ||
                unfinishedInst.opcode == Sll ||
                unfinishedInst.opcode == Sra ||
                unfinishedInst.opcode == Srl ||
                unfinishedInst.opcode == Lw)
                    return true;
        }
    }

    // Current Rd RAW hazards
    if (instNow.rdIdx == unfinishedInst.rdIdx   &&
        instNow.rdIdx != ZERO_REGISTER_INDEX    &&
        instNow.rdIdx != IMMEDIATE_REGISTER_INDEX)
    {
        // Only these opcodes need to read rd at a later stage, and therefore should cause a stall
        if (instNow.opcode == Beq ||
            instNow.opcode == Bne ||
            instNow.opcode == Blt ||
            instNow.opcode == Bgt ||
            instNow.opcode == Ble ||
            instNow.opcode == Bge ||
            instNow.opcode == Jal ||
            instNow.opcode == Sw)
        {
            // Rd is updated only in these instructions
            if (unfinishedInst.opcode == Add ||
                unfinishedInst.opcode == Sub ||
                unfinishedInst.opcode == And ||
                unfinishedInst.opcode == Or  ||
                unfinishedInst.opcode == Xor ||
                unfinishedInst.opcode == Mul ||
                unfinishedInst.opcode == Sll ||
                unfinishedInst.opcode == Sra ||
                unfinishedInst.opcode == Srl ||
                unfinishedInst.opcode == Lw)
                    return true;
        }
    }

    return false;
}

bool isDataHazard(DecodeStage* self)
{
    if (*self->state.myCore->p_cycle >= 100 && self->state.myCore->id == 2)
        printf("hi");
    if (*self->state.myCore->p_cycle >= 47 && self->state.myCore->id == 2)
        printf("hi");

    return  check_RAW_Hazard(self->state.inputState.instruction, self->state.myCore->execute_stage.state.inputState.instruction) ||
            check_RAW_Hazard(self->state.inputState.instruction, self->state.myCore->memory_stage.state.inputState.instruction) ||
            check_RAW_Hazard(self->state.inputState.instruction, self->state.myCore->writeback_stage.state.inputState.instruction); 
}

void updatePCtoRdValue(DecodeStage* self)
{
    // Update PC to branch target (lower 10 bits of rd)
    self->state.myCore->pc_register.updated = self->state.outputState.rdValue & 0x3FF;
    self->state.myCore->pc_registered_updated_by = 1;
}

/**
 * The function attempts to handle an edge case where the halt command is located in the delay slot.
 * In such a case, we would not like the pc value to change to the jump target in case the jump was taken.
 * Instead, we wish for the pc value to stay constant.
 */
void handleEdgeCaseOfHaltInDelaySlot(DecodeStage* self)
{
    uint32_t fetchedInstructionCodex = self->state.myCore->instruction_memory[self->state.myCore->pc_register.now];
    Instruction fetchedInst = create_instruction(fetchedInstructionCodex);
    if (fetchedInst.opcode == Halt)
    {
        self->state.myCore->pc_register.updated = self->state.myCore->pc_register.now;
        self->state.myCore->pc_registered_updated_by = 1;
    }
}

void doOperationsOfJumpInstructions(DecodeStage* self)
{
    switch (self->state.inputState.instruction.opcode)
    {
        case Beq:
        {
            if (self->state.outputState.rsValue == self->state.outputState.rtValue)
            {
                updatePCtoRdValue(self);
            }
            break;
        }
        case Bne:
        {
            if (self->state.outputState.rsValue != self->state.outputState.rtValue)
            {
                updatePCtoRdValue(self);
            }
            break;
        }
        case Blt:
        {
            if (self->state.outputState.rsValue < self->state.outputState.rtValue)
            {
                updatePCtoRdValue(self);
            }
            break;
        }
        case Bgt:
        {
            if (self->state.outputState.rsValue > self->state.outputState.rtValue)
            {
                updatePCtoRdValue(self);
            }
            break;
        }
        case Ble:
        {
            if (self->state.outputState.rsValue <= self->state.outputState.rtValue)
            {
                updatePCtoRdValue(self);
            }
            break;
        }
        case Bge:
        {
            if (self->state.outputState.rsValue >= self->state.outputState.rtValue)
            {
                updatePCtoRdValue(self);
            }
            break;
        }
        case Jal:
        {
            updatePCtoRdValue(self);
            self->state.myCore->registers[15].updated = self->state.myCore->pc_register.now + 1;
            break;
        }
        default:
            break;
    }

    if (self->state.inputState.instruction.opcode == Beq ||
        self->state.inputState.instruction.opcode == Bne ||
        self->state.inputState.instruction.opcode == Blt ||
        self->state.inputState.instruction.opcode == Ble ||
        self->state.inputState.instruction.opcode == Bgt ||
        self->state.inputState.instruction.opcode == Bge ||
        self->state.inputState.instruction.opcode == Jal )
    {
        handleEdgeCaseOfHaltInDelaySlot(self);
    }
}

bool do_decode_operation(DecodeStage* self)
{
    if (self->state.myCore->id == 2)
        printf("decode: %d cycle: %d\n", self->state.inputState.instructionAddress, (int)*(self->state.myCore->p_cycle));

    // First copy the output state from the input state.
    // Later update the output state with operation needed to be
    // done at this round
    self->state.outputState = self->state.inputState;

    if (isDataHazard(self))
    {
        self->state.inputState.is_ready = true;
        self->state.outputState.is_ready = false;
        return true;
    }

    updateRegisterValues_decode(self);

    doOperationsOfJumpInstructions(self);

    self->state.inputState.is_ready = false;
    self->state.outputState.is_ready = true;
    return false;
}

void configure_decode_stage(DecodeStage* stage, struct core* myCore)
{
    configure_stage_data(&(stage->state), myCore);
}