#include "main_memory.h"
#include <stdio.h>

// Function to create and initialize the main memory
MainMemory main_memory_create() {
    MainMemory mem;

    // Initialize all memory rows to zero
    for (unsigned int i = 0; i < MEMORY_SIZE; ++i) {
        mem.memory[i] = 0;
    }

    return mem;
}

// Function to read a word from the memory at a specific address
unsigned int main_memory_read(const MainMemory* mem, unsigned int address) {
    if (address >= MEMORY_SIZE) {
        fprintf(stderr, "Error: Address out of bounds (read: 0x%X)\n", address);
        return 0; // Return 0 for out-of-bounds addresses
    }
    return mem->memory[address];
}

// Function to write a word to the memory at a specific address
void main_memory_write(MainMemory* mem, unsigned int address, unsigned int data) {
    if (address >= MEMORY_SIZE) {
        fprintf(stderr, "Error: Address out of bounds (write: 0x%X)\n", address);
        return;
    }
    mem->memory[address] = data;
}
