#ifndef BUS_SNOOPER_H
#define BUS_SNOOPER_H

#include "flip_flop.h"
#include "cache_block.h"

struct busManager;
struct core;

// BusSnooper structure definition
typedef struct {
    FlipFlop_bool busSnooperActive;  // Indicates if the BusSnooper is active in the current
    uint32_t addr_to_flush;     // Address to flush
    int start_flush_cycle;          // Cycle when the flush started

    struct core* myCore;
} BusSnooper;

// Function to initialize a BusSnooper on the heap
void configure_bus_snooper(BusSnooper* snooper, struct core* myCore);

// Function for the BusSnooper to snoop and react to bus commands
void do_snoop_operation(BusSnooper* snooper);

#endif

