#include "core.h"


// Function to initialize a Core instance
Core core_create(int id) {
    Core newCore;

    newCore.id = id

    // Initialize program counters to 0
    newCore.pc_register_now = 0;
    newCore.pc_register_updated = 0;

    // Initialize all registers to 0
    memset(newCore.registers_now, 0, sizeof(newCore.registers_now));
    memset(newCore.registers_updated, 0, sizeof(newCore.registers_updated));

    // Initialize InstructionMemory to 0
    memset(newCore.instructionMemory, 0, sizeof(newCore.instructionMemory));

    // Allocate memory for caches and initialize them
    newCore.cache_now = (Cache*)malloc(sizeof(Cache));
    newCore.cache_updated = (Cache*)malloc(sizeof(Cache));

    if (newCore.cache_now) {
        *newCore.cache_now = cacheCreate(); // Initialize cache_now
    }
    if (newCore.cache_updated) {
        *newCore.cache_updated = cacheCreate(); // Initialize cache_updated
    }

    return newCore;
}

// Function to free allocated resources in a Core instance
void core_destroy(Core* core) {
    if (core->cache_now) {
        free(core->cache_now);
        core->cache_now = NULL;
    }
    if (core->cache_updated) {
        free(core->cache_updated);
        core->cache_updated = NULL;
    }
}
