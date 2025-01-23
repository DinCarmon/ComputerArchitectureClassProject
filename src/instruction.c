#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "instruction.h"

/**
 * Parses a 32-bit instruction into its components
 * @param codex 32-bit instruction
 * @return Parsed instruction object
 */
void parseCmdToInstruction(Instruction* self, uint32_t codex)
{
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

    if (codex > Halt) // Assume all codexes are valid until halt, and not above
    {
        printf("Unrecognized codex");
        exit(1);
    }
    self->opcode = codex;
}

/**
 * @param instructionCodex The instruction codex
 * @return the instruction parsed. 
 */
Instruction create_instruction(uint32_t instructionCodex)
{
    Instruction inst;
    inst.parse_cmd = parseCmdToInstruction;
    inst.parse_cmd(&inst, instructionCodex);
    return inst;
}

void configure_instruction(Instruction* inst)
{
    inst->codex = 0;
    inst->immediate = 0;
    inst->opcode = 0;
    inst->parse_cmd = 0;
    inst->rdIdx = 0;
    inst->rtIdx = 0;
    inst->rsIdx = 0;
}