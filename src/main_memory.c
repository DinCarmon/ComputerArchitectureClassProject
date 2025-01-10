#include "main_memory.h"

// Function to create and initialize MainMemory
MainMemory* create_main_memory() {
    // Allocate memory for MainMemory structure
    MainMemory* main_memory = (MainMemory*)malloc(sizeof(MainMemory));
    if (!main_memory) {
        return NULL; // Return NULL if memory allocation fails
    }

    // Initialize all memory rows to zero
    for (size_t i = 0; i < MEMORY_SIZE; i++) {
        main_memory->memory[i] = 0;
    }

    return main_memory;
}

// Destroy function to free MainMemory
void main_memory_destroy(MainMemory* mem) {
    if (mem == NULL) {
        return;
    }
    free(mem);
}

// Function to read from MainMemory
unsigned int main_memory_read(MainMemory* mem, unsigned int address) {
    if (address >= MEMORY_SIZE) {
        fprintf(stderr, "Address out of bounds: %u\n", address);
        exit(EXIT_FAILURE);
    }
    return mem->memory[address];
}

// Function to write to MainMemory
void main_memory_write(MainMemory* mem, unsigned int address, unsigned int data) {
    if (address >= MEMORY_SIZE) {
        fprintf(stderr, "Address out of bounds: %u\n", address);
        exit(EXIT_FAILURE);
    }
    mem->memory[address] = data;
}
