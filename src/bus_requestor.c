#include "bus_requestor.h"

// Function to create and initialize a BusRequestor on the heap
BusRequestor* bus_requestor_create(int id) {
    BusRequestor* requestor = (BusRequestor*)malloc(sizeof(BusRequestor));
    if (!requestor) {
        fprintf(stderr, "Failed to allocate memory for BusRequestor.\n");
        exit(EXIT_FAILURE);
    }

    // Initialize BusRequestor fields
    requestor->operation = 0;  // Default operation
    requestor->address = 0;    // Default address
    requestor->start_cycle = 0; // Default start cycle
    requestor->IsAlreadyAskedForBus.now = false;
    requestor->IsAlreadyAskedForBus.updated = false;
    requestor->HaveTransactionOnBus.now = false;
    requestor->HaveTransactionOnBus.updated = false;
    requestor->IsRequestOnBus.now = false;
    requestor->IsRequestOnBus.updated = false;
    requestor->LastCycle.now = false;
    requestor->LastCycle.updated = false;
    requestor->priority = -1; // Default priority
    requestor->id = id; // Set the provided ID

    return requestor;
}

// Function to check if the BusRequestor is already occupied
bool BusRequestorAlreadyOccupied(const BusRequestor* requestor) {
    return requestor->IsRequestOnBus.now;
}

// Function to check if the request is over, if it is resest the requestor
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
    requestor->IsAlreadyAskedForBus.now = false;
    requestor->IsAlreadyAskedForBus.updated = false;
    requestor->HaveTransactionOnBus.now = false;
    requestor->HaveTransactionOnBus.updated = false;
    requestor->IsRequestOnBus.now = false;
    requestor->IsRequestOnBus.updated = false;
    requestor->LastCycle.now = false;
    requestor->LastCycle.updated = false;
    //requestor->priority = 0;       // Reset priority
}

// Function to request an action from the bus (sets the request operation and address)
// XXX
void Enlist(BusRequestor* requestor, int addr, int BusActionType) {
    // Set the operation type and address if the requestor is free
    requestor->operation = BusActionType;
    requestor->address = addr;

    // Mark the requestor as occupied
    // XXX: Call RequestForBus
    }
}

// Function to destroy a BusRequestor
void bus_requestor_destroy(BusRequestor* requestor) {
    if (requestor) {
        free(requestor);
    }
}

// Function to perform the bus operation if granted
//void DoOperation(BusRequestor* requestor, BusManager* manager, int cycle) {
//    // Check if the requestor is occupied (if not, return)
//    if (!requestor->busRequestorOccupied_now) {
//        return;
//    }
//
//    // Check if the requestor is not currently in transaction and if the bus manager grants the request
//    if (!requestor->busRequestInTransaction_now && manager->RequestForBusGrantedRightNow()) {
//        // Set the requestor's transaction as in progress
//        requestor->busRequestInTransaction_now = true;
//
//        // Set the start cycle of the request
//        requestor->start_cycle = cycle;
//
//        // Set the manager's bus details based on the requestor's request
//        manager->bus_origid = requestor->id;
//        manager->bus_cmd = requestor->operation;
//        manager->bus_addr = requestor->address;
//    }
//}