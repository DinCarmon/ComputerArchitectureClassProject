#include <stdint.h>
#include "instruction.h"

/**
 * Parses a 32-bit instruction into its components
 * @param codex 32-bit instruction
 * @return Parsed instruction object
 */
void parseCmdToInstruction(Instruction* self, uint32_t codex) {
    self->codex = codex;

    self->immediate = codex % (uint32_t)(pow(2, IMMEDIATE_FIELD_SIZE_IN_BITS));
    codex /= (uint32_t)(pow(2, IMMEDIATE_FIELD_SIZE_IN_BITS));

    self->rtIdx = codex % (uint32_t)(pow(2, REGISTER_INDEX_FIELD_SIZE_IN_BITS));
    codex /= (uint32_t)(pow(2, REGISTER_INDEX_FIELD_SIZE_IN_BITS));

    self->rsIdx = codex % (uint32_t)(pow(2, REGISTER_INDEX_FIELD_SIZE_IN_BITS));
    codex /= (uint32_t)(pow(2, REGISTER_INDEX_FIELD_SIZE_IN_BITS));

    self->rdIdx = codex % (uint32_t)(pow(2, REGISTER_INDEX_FIELD_SIZE_IN_BITS));
    codex /= (uint32_t)(pow(2, REGISTER_INDEX_FIELD_SIZE_IN_BITS));

    // Sign extension for immediate value
    if (self->immediate > (uint32_t)(pow(2, IMMEDIATE_FIELD_SIZE_IN_BITS) / 2))
    {
        self->immediate |= 0xFFFFF000;
    }
}

/**
 * @param instructionCodex The instruction codex
 * @return the instruction parsed. 
 */
Instruction createInstruction(uint32_t instructionCodex)
{
    Instruction inst;
    inst.parseCmd = parseCmdToInstruction;
    inst.parseCmd(&inst, instructionCodex);
    return inst;
}
