#include "bus_requestor.h"

// Function to create and initialize a BusRequestor
BusRequestor bus_requestor_create(int id) {
    BusRequestor requestor;

    // Initialize BusRequest
    requestor.request.operation = 0;
    requestor.request.address = 0;
    requestor.request.start_cycle = 0;

    // Initialize boolean flags
    requestor.busRequestorOccupied_now = false;
    requestor.busRequestorOccupied_updated = false;
    requestor.busRequestInTransaction_now = false;

    // Initialize priority
    requestor.priority = 0;

    // Set the id
    requestor.id = id;

    return requestor;
}

// Function to check if the BusRequestor is already occupied
bool BusRequestorAlreadyOccupied(const BusRequestor* requestor) {
    return requestor->busRequestorOccupied_now;
}

// Function to reset the BusRequestor's request data
void resetBusRequestor(BusRequestor* requestor) {
    if (!requestor) {
        return; // Handle null pointer
    }

    // Reset the request data
    requestor->request.operation = 0;
    requestor->request.address = 0;
    requestor->request.start_cycle = 0;
   
    // Reset the boolean flags
    requestor->busRequestorOccupied_now = false;
    requestor->busRequestorOccupied_updated = false;
    requestor->busRequestInTransaction_now = false;

    // Reset the priority
    requestor->priority = 0;
}
