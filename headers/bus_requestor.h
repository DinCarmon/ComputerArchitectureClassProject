#pragma once
#include <stdbool.h>
#include "bus_manager.h"  // Include bus_manager.h for BusManager and RequestForBusGrantedRightNow()

// Definition of the BusRequestor struct
typedef struct {
    int operation;               // Operation type (e.g., read, write, invalidate)
    int address;                 // Address associated with the bus request
    int start_cycle;             // Cycle when the request started

    bool busRequestorOccupied_now;    // True if a bus request is waiting to be sent or in progress
    bool busRequestorOccupied_updated; // True if the requestor's status was recently updated
    bool busRequestInTransaction_now;  // True if a bus request was already sent, waiting to end
    int priority;                     // Priority of the bus requestor
    int id;                           // Identifier for the BusRequestor
} BusRequestor;

// Function to create and initialize a BusRequestor
BusRequestor bus_requestor_create(int id);

// Function to check if the BusRequestor is already occupied
bool BusRequestorAlreadyOccupied(const BusRequestor* requestor);

// Function to reset the BusRequestor to its initial state
void bus_requestor_reset(BusRequestor* requestor);

// Function to request an action from the bus (sets the request operation and address)
void RequestActionFromBus(BusRequestor* requestor, int addr, int BusActionType);

// Function to perform the bus operation if granted
void DoOperation(BusRequestor* requestor, BusManager* manager, int cycle);
