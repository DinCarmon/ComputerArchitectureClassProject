#pragma once
#include <stdint.h> // For uint32_t
#include <stdlib.h> // For malloc and free
#include <string.h> // For memset
#include "cache.h"  // Include the Cache definition

// Define the Core struct
typedef struct {
    int id                            //core id
    int pc_register_now;              // Current program counter
    int pc_register_updated;         // Updated program counter
    uint32_t registers_now[32];      // Current state of registers
    uint32_t registers_updated[32];  // Updated state of registers
    int InstructionMemory[1024][1];  // Instruction memory array
    Cache* cache_now;                // Pointer to the current cache
    Cache* cache_updated;            // Pointer to the updated cache
} Core;

// Function to initialize a Core instance
Core core_create(int id);

// Function to free allocated resources in a Core instance
void core_destroy(Core* core);
