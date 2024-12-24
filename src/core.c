#include "core.h"


// Function to initialize a Core instance
Core core_create(void) {
    Core newCore;

    // Initialize program counters to 0
    newCore.pc_register_now = 0;
    newCore.pc_register_updated = 0;

    // Initialize all registers to 0
    memset(newCore.registers_now, 0, sizeof(newCore.registers_now));
    memset(newCore.registers_updated, 0, sizeof(newCore.registers_updated));

    // Initialize InstructionMemory to 0
    memset(newCore.InstructionMemory, 0, sizeof(newCore.InstructionMemory));

    // Allocate memory for caches and initialize them
    newCore.cache_now = (Cache*)malloc(sizeof(Cache));
    newCore.cache_updated = (Cache*)malloc(sizeof(Cache));

    if (newCore.cache_now) {
        *newCore.cache_now = cache_create(); // Initialize cache_now
    }
    if (newCore.cache_updated) {
        *newCore.cache_updated = cache_create(); // Initialize cache_updated
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
