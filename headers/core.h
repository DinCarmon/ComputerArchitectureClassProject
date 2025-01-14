#pragma once
#include <stdio.h>
#include <stdlib.h> // For malloc and free
#include <string.h> // For memset
#include "cache.h"  // Include the Cache definition
#include "flip_flop.h"
#include "defines.h"


// Define the Core struct
typedef struct {
    int id;                          //core id
    FlipFlop_uint32_t pc_register;    // flip flop to hold the program counter
    FlipFlop_uint32_t registers[32];            //flip flop to hold the state of the registers
    uint32_t InstructionMemory[1024];  // Instruction memory array
    Cache* cache_now;                // Pointer to the current cache
    Cache* cache_updated;            // Pointer to the updated cache
} Core;

// Function to initialize a Core instance
Core* core_create(int id);

// Function to free allocated resources in a Core instance
void core_destroy(Core* core);

// Function to advance the core to the next cycle
void advance_core(Core* core, bool keep_value);
