#include "bus_requestor.h"


// Function to create and initialize a BusRequestor on the heap
BusRequestor bus_requestor_create(int id) {
    BusRequestor requestor;

    // Initialize BusRequestor fields
    requestor.operation = 0;  // Default operation
    requestor.address = 0;    // Default address
    requestor.start_cycle = 0; // Default start cycle
    requestor.IsRequestOnBus.now = false;
    requestor.IsRequestOnBus.updated = false;
    requestor.LastCycle.now = false;
    requestor.LastCycle.updated = false;
    requestor.priority = -1; // Default priority
    requestor.id = id; // Set the provided ID

    return requestor;
}

// Function to check if the BusRequestor is already occupied
bool BusRequestorAlreadyOccupied(const BusRequestor* requestor) {
    return requestor->IsRequestOnBus.now;
}

// Function to check if the request is over, if it is reset the requestor
bool BusRequestOver(BusRequestor* requestor) {
    if (requestor->LastCycle.now) {
        bus_requestor_reset(requestor);
        return true;
    }
    return false;
}

// Function to reset the BusRequestor to its initial state
void bus_requestor_reset(BusRequestor* requestor) {
    if (!requestor) {
        return; // Return if the pointer is null
    }

    // Reset all fields to their default state
    requestor->operation = 0;      // Reset operation
    requestor->address = 0;        // Reset address
    requestor->start_cycle = 0;    // Reset start cycle
    requestor->IsRequestOnBus.now = false;
    requestor->IsRequestOnBus.updated = false;
    requestor->LastCycle.now = false;
    requestor->LastCycle.updated = false;
    //requestor->priority = 0;       // Reset priority
}