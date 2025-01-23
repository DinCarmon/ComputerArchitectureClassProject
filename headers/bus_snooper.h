#ifndef BUS_SNOOPER_H
#define BUS_SNOOPER_H

#include "flip_flop.h"
#include "cache_block.h"

struct busManager;
struct core;

// BusSnooper structure definition
typedef struct {

    struct core* myCore;
} BusSnooper;

// Function to initialize a BusSnooper on the heap
void configure_bus_snooper(BusSnooper* snooper, struct core* myCore);

// Function for the BusSnooper to snoop and react to bus commands.
// The function only triggered if it sees something related to it on the bus.
// After that, it interrupts the bus. It is the bus job to write following flushes from the interuptor cache.
void do_snoop_operation(BusSnooper* snooper);

#endif

