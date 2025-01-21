#pragma once
#include "defines.h" 
#include "flip_flop.h"

// BusSnooper structure definition
typedef struct {
    FlipFlop_bool busSnooperActive;  // Indicates if the BusSnooper is active in the current
    uint32_t addr_to_flush;     // Address to flush
    int start_flush_cycle;          // Cycle when the flush started
    int id;                         // Identifier for this BusSnooper. core id
} BusSnooper;

// Function to create and initialize a BusSnooper on the heap
BusSnooper* bus_snooper_create(int id);

// Function to destroy a BusSnooper and free its memory
void bus_snooper_destroy(BusSnooper* snooper);


