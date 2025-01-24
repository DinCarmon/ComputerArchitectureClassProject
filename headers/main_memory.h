#ifndef MAIN_MEMORY_H
#define MAIN_MEMORY_H

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include "cache_block.h"

#define MEMORY_SIZE (1 << 20) // 2^20 rows of uint32_t

typedef struct {
    uint32_t memory[MEMORY_SIZE]; // Main memory array
} MainMemory;

// Function to create and initialize the main memory
void reset_main_memory(MainMemory* mem);

#endif