#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdint.h>
#include "constants.h"

/**
 * Instruction - Represents a single CPU instruction
 * Format: [31:24] opcode, [23:20] rd, [19:16] rs, [15:12] rt, [11:0] immediate
 */
typedef struct instruction {
    OpcodeType      opcode;             // Operation type
    uint8_t         rdIdx;              // Destination register
    uint8_t         rsIdx;              // Source register 1
    uint8_t         rtIdx;              // Source register 2
    uint32_t        immediate;          // Immediate value
    uint32_t        codex;              // Codex - original representation of the instruction

    void (*parse_cmd)(struct instruction* self, uint32_t codex);
} Instruction;

Instruction create_instruction(uint32_t instructionCodex);

void configure_instruction(Instruction* inst);

#endif