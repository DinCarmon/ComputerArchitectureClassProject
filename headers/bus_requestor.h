#ifndef BUS_REQUESTOR_H
#define BUS_REQUESTOR_H

#include <stdio.h>
#include <stdlib.h>
#include "flip_flop.h"


// Definition of the BusRequestor struct
typedef struct {
    uint32_t operation;                       // Operation type (e.g., read, write, invalidate)
    uint32_t address;                        // Address associated with the bus request
    int start_cycle;                    // Cycle when the request started
    FlipFlop_bool IsRequestOnBus;       // is the request granted
    FlipFlop_bool LastCycle;             // is it the last cycle of the request. // 
    bool busRequestInTransaction_now;  // True if a bus request was already sent, waiting to end
    int priority;                     // Priority of the bus requestor
    int id;                           // Identifier for the BusRequestor
} BusRequestor;
// Function to create and initialize a BusRequestor
BusRequestor bus_requestor_create(int id);

// Function to check if the request is over, if it is resest the requestor
bool BusRequestOver(BusRequestor* requestor);

// Function to check if the BusRequestor is already occupied
bool BusRequestorAlreadyOccupied(const BusRequestor* requestor);

// Function to reset the BusRequestor to its initial state
void bus_requestor_reset(BusRequestor* requestor);

#endif