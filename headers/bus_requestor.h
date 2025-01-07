#ifndef BUS_REQUESTOR_H
#define BUS_REQUESTOR_H

#include <stdbool.h>
#include "flip_flop.h"
#include "constants.h"
#include "bus_manager.h"  // Include bus_manager.h for BusManager and RequestForBusGrantedRightNow()

// Definition of the BusRequestor struct
typedef struct {
    FlipFlop_CacheLineStatus operation;             // Operation type (e.g., read, write, invalidate)
    FlipFlop_uint32_t address;                      // Address associated with the bus request
    FlipFlop_bool busRequestorOccupied;             // True if a bus request is waiting to be sent or in progress
    FlipFlop_bool busRequestInTransaction;          // True if a bus request was already sent, waiting to end
    FlipFlop_uint32_t priority;                     // Priority of the bus requestor
    
    int id;                                         // Identifier for the BusRequestor
} BusRequestor;

// Function to create and initialize a BusRequestor
BusRequestor createBusRequestor(int id);

// Function to check if the BusRequestor is already occupied
bool busRequestorAlreadyOccupied(const BusRequestor* requestor);

// Function to reset the BusRequestor to its initial state
void resetBusRequestor(BusRequestor* requestor);

// Function to request an action from the bus (sets the request operation and address)
void RequestActionFromBus(BusRequestor* requestor, int addr, int BusActionType);

// Function to perform the bus operation if granted
void DoOperation(BusRequestor* requestor, BusManager* manager);

#endif