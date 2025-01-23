#include "bus_requestor.h"
#include "core.h"

// Function to create and initialize a BusRequestor on the heap
void configure_bus_requestor(BusRequestor* requestor, struct core* myCore)
{
    // Initialize BusRequestor fields
    requestor->operation = NO_CMD;  // Default operation
    requestor->address = 0;    // Default address
    requestor->priority = myCore->id; // Default priority

    requestor->myCore = myCore;
}