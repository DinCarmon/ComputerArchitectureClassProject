#include "core.h"


// Function to initialize a Core instance on the heap
Core* core_create(int id) {
    // Allocate memory for the Core structure
    Core* newCore = (Core*)malloc(sizeof(Core));
    if (newCore == NULL) {
        fprintf(stderr, "Memory allocation failed for Core\n");
        exit(EXIT_FAILURE);
    }

    // Initialize fields
    newCore->id = id;
    newCore->pc_register.now = 0;
    newCore->pc_register.updated = 0;
    memset(newCore->registers, 0, sizeof(newCore->registers));
    memset(newCore->InstructionMemory, 0, sizeof(newCore->InstructionMemory));

    // Allocate memory for caches
    newCore->cache_now = (Cache*)malloc(sizeof(Cache));
    newCore->cache_updated = (Cache*)malloc(sizeof(Cache));

    if (newCore->cache_now == NULL || newCore->cache_updated == NULL) {
        fprintf(stderr, "Memory allocation failed for caches\n");
        free(newCore); // Free the Core structure if cache allocation fails
        exit(EXIT_FAILURE);
    }

    // Initialize the caches
    *newCore->cache_now = cache_create();
    *newCore->cache_updated = cache_create();

    return newCore;
}

// Function to destroy a Core instance and free all memory
void core_destroy(Core* core) {
    if (core == NULL) {
        return; // Do nothing if the pointer is NULL
    }

    // Free allocated memory for caches
    free(core->cache_now);
    free(core->cache_updated);

    // Free the Core structure itself
    free(core);
}


// Function to advance the core to the next cycle
void advance_core(Core* core, bool keep_value) {
    UPDATE_FLIP_FLOP(core->pc_register, keep_value);
    for (int i = 0; i < 32; i++) {
        UPDATE_FLIP_FLOP(core->registers[i], keep_value);
    }
}

