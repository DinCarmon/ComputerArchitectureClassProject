#pragma once
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include "defines.h"
#include "cache.h"

#define MEMORY_SIZE (1 << 20) // 2^20 rows of uint32_t

typedef struct {
    uint32_t memory[MEMORY_SIZE]; // Main memory array
} MainMemory;

// Function to create and initialize the main memory
MainMemory* create_main_memory();

// Destroy function to free MainMemory
void main_memory_destroy(MainMemory* mem);

// Function to read a block from cache to the main memory
void main_memory_read(MainMemory* mem, Cache* cache, uint32_t address, uint32_t state);

// Function to write a block from a cache to the main memory
void main_memory_write(MainMemory* mem, Cache* cache, uint32_t address);
