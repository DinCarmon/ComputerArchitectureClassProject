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
void main_memory_read(MainMemory* mem, Cache* cache, uint32_t address, uint32_t state) {
    if (address >= MEMORY_SIZE) {
        fprintf(stderr, "Address out of bounds: %u\n", address);
        exit(EXIT_FAILURE);
    }
    update_state(address, cache, state);
    int offset = get_block_offset(address);
    uint32_t mod_address = address - offset;
    for (int i = 0; i < BLOCK_OFFSET_SIZE; ++i) {
        mod_address = mod_address + i;
        uint32_t data = mem->memory[mod_address];
        int j = write_cache(mod_address, cache, data);

    }

}

// Function to write to MainMemory
void main_memory_write(MainMemory* mem, Cache* cache, uint32_t address) {
    if (address >= MEMORY_SIZE) {
        fprintf(stderr, "Address out of bounds: %u\n", address);
        exit(EXIT_FAILURE);
    }
    int offset = get_block_offset(address);
    uint32_t mod_address = address - offset;
    for (int i = 0; i < BLOCK_OFFSET_SIZE; ++i) {
        mod_address = mod_address + i;
        uint32_t data = read_cache(mod_address, cache);
        mem->memory[mod_address] = data;
        
    }
}
