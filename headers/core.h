#ifndef CORE_H
#define CORE_H

#include <stdint.h> // For uint32_t
#include <stdlib.h> // For malloc and free
#include <string.h> // For memset
#include "cache.h"  // Include the Cache definition
#include "flip_flop.h"
#include "constants.h"

// Define the Core struct
typedef struct core {
    int id;                                                             // Core id
    struct FlipFlop pcRegister;                                         // program counter
    struct FlipFlop registers[NUM_REGISTERS_PER_CORE];                  // Current state of registers
    struct FlipFlop instructionMemory[INSTRUCTION_MEMORY_DEPTH];        // Instruction memory array
    Cache* cache;                                                       // Pointer to the cache
} Core;

// Function to initialize a Core instance
Core core_create(int id);

// Function to free allocated resources in a Core instance
void core_destroy(Core* core);

#endif