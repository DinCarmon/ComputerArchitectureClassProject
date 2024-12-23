#pragma once
#include <stdbool.h>

typedef struct {
    int operation;    // The operation to be performed by the requestor
    int address;      // The address involved in the bus transaction
    int start_cycle;  // The cycle at which the bus request starts
} BusRequest;

// Definition of the BusRequestor struct
typedef struct {
    BusRequest request;                // Current bus request
    bool busRequestorOccupied_now;     // True if a bus request is waiting to be sent or in progress
    bool busRequestorOccupied_updated; // Indicates if the requestor has been updated
    bool busRequestInTransaction_now;  // True if a bus request was already sent and is waiting to end
    int priority;                      // Priority of the bus requestor
    int id;                            // Identifier for the BusRequestor
} BusRequestor;

// Function to create and initialize a BusRequestor
BusRequestor bus_requestor_create(int id);

// Function to check if the BusRequestor is already occupied
bool BusRequestorAlreadyOccupied(const BusRequestor* requestor);

// Function to reset the BusRequestor's request data
void resetBusRequestor(BusRequestor* requestor);
