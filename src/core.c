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
    newCore->pc_register_now = 0;
    newCore->pc_register_updated = 0;
    memset(newCore->registers_now, 0, sizeof(newCore->registers_now));
    memset(newCore->registers_updated, 0, sizeof(newCore->registers_updated));
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

