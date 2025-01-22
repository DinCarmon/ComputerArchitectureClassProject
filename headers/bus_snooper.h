#ifndef BUS_SNOOPER_H
#define BUS_SNOOPER_H

#include "flip_flop.h"
#include "cache_block.h"

struct busManager;

// BusSnooper structure definition
typedef struct {
    FlipFlop_bool busSnooperActive;  // Indicates if the BusSnooper is active in the current
    uint32_t addr_to_flush;     // Address to flush
    int start_flush_cycle;          // Cycle when the flush started
    int id;                         // Identifier for this BusSnooper. core id
} BusSnooper;

// Function to create and initialize a BusSnooper on the heap
BusSnooper bus_snooper_create(int id);

// Function for the BusSnooper to snoop and react to bus commands
void snoop(BusSnooper* snooper, Cache* cache, struct busManager* manager);

#endif

