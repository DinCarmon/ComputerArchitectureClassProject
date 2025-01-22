#include "main_memory.h"

// Function to create and initialize MainMemory
void reset_main_memory(MainMemory* mem)
{
    // Initialize all memory rows to zero
    for (size_t i = 0; i < MEMORY_SIZE; i++) {
        mem->memory[i] = 0;
    }
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
        write_cache(mod_address, cache, data);

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
