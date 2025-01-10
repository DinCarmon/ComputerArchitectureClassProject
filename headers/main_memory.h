#pragma once
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include "defines.h"

#define MEMORY_SIZE (1 << 20) // 2^20 rows of unsigned int

typedef struct {
    unsigned int* memory[MEMORY_SIZE]; // Main memory array
} MainMemory;

// Function to create and initialize the main memory
MainMemory* create_main_memory();

// Destroy function to free MainMemory
void main_memory_destroy(MainMemory* mem);

// Function to read a word from the memory at a specific address
unsigned int main_memory_read(const MainMemory* mem, unsigned int address);

// Function to write a word to the memory at a specific address
void main_memory_write(MainMemory* mem, unsigned int address, unsigned int data);
