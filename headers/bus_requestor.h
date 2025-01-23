#ifndef BUS_REQUESTOR_H
#define BUS_REQUESTOR_H

#include <stdio.h>
#include <stdlib.h>
#include "flip_flop.h"

struct core;

// Definition of the BusRequestor struct
typedef struct {
    BusCmd operation;                           // Operation type (e.g., read, write, invalidate)
    uint32_t address;                           // Address associated with the bus request
    int priority;                               // Priority of the bus requestor
    
    struct core* myCore;
} BusRequestor;

// Function to create and initialize a BusRequestor
void configure_bus_requestor(BusRequestor* bus_requestor, struct core* myCore);

#endif