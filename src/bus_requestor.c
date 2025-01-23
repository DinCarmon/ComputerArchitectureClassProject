#include "bus_requestor.h"
#include "core.h"

// Function to create and initialize a BusRequestor on the heap
void configure_bus_requestor(BusRequestor* requestor, struct core* myCore)
{
    // Initialize BusRequestor fields
    requestor->operation = NO_CMD;  // Default operation
    requestor->address = 0;    // Default address
    requestor->start_cycle = 0; // Default start cycle
    requestor->is_request_on_bus.now = false;
    requestor->is_request_on_bus.updated = false;
    requestor->priority = myCore->id; // Default priority

    requestor->myCore = myCore;
}

// Function to check if the BusRequestor is already occupied
bool BusRequestorAlreadyOccupied(const BusRequestor* requestor) {
    return requestor->is_request_on_bus.now;
}

// Function to reset the BusRequestor to its initial state
void bus_requestor_reset(BusRequestor* requestor) {
    if (!requestor) {
        return; // Return if the pointer is null
    }

    // Reset all fields to their default state
    requestor->operation = NO_CMD;      // Reset operation
    requestor->address = 0;        // Reset address
    requestor->start_cycle = 0;    // Reset start cycle
    requestor->is_request_on_bus.now = false;
    requestor->is_request_on_bus.updated = false;

}