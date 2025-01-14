#pragma once
#include "defines.h"  
#include "core.h"
#include "bus_manager.h"
#include "cache.h"

// BusSnooper structure definition
typedef struct {
    Core* my_core;                  // Pointer to the core associated with this BusSnooper
    BusManager* manager;            // Pointer to the BusManager
    FlipFlop_bool busSnooperActive;  // Indicates if the BusSnooper is active in the current
    uint32_t addr_to_flush;     // Address to flush
    int start_flush_cycle;          // Cycle when the flush started
    int id;                         // Identifier for this BusSnooper
} BusSnooper;

// Function to create and initialize a BusSnooper on the heap
BusSnooper* bus_snooper_create(Core* my_core, BusManager* manager, int id);

// Function to destroy a BusSnooper and free its memory
void bus_snooper_destroy(BusSnooper* snooper);

// Function for the BusSnooper to snoop and react to bus commands
void snoop(BusSnooper* snooper, Cache* cache);
