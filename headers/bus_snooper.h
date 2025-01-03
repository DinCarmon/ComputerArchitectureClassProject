#pragma once
#include <stdbool.h>
#include "core.h"
#include "bus_manager.h"
#include "cache.h"

// BusSnooper structure definition
typedef struct {
    Core* my_core;                  // Pointer to the core associated with this BusSnooper
    BusManager* manager;            // Pointer to the BusManager
    bool busSnooperActive_now;      // Does the BusSnooper need to answer something right now
    bool busSnooperActive_updated;  // For the next cycle
    unsigned int addr_to_flush;     // Address to flush
    int start_flush_cycle;          // Cycle when flush started
    int id;                         // Identifier for this BusSnooper
} BusSnooper;

// Function to create and initialize a BusSnooper
BusSnooper bus_snooper_create(Core* my_core, BusManager* manager, int id);

// Function for the BusSnooper to snoop and react to bus commands
void snoop(BusSnooper* snooper, Cache* cache);
