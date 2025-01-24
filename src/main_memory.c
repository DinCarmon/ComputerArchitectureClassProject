#include "main_memory.h"

// Function to create and initialize MainMemory
void reset_main_memory(MainMemory* mem)
{
    // Initialize all memory rows to zero
    for (size_t i = 0; i < MEMORY_SIZE; i++) {
        mem->memory[i] = 0;
    }
}