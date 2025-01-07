#include "bus_requestor.h"

// Function to create and initialize a BusRequestor
BusRequestor createBusRequestor(int id) {
    BusRequestor requestor;

    // Initialize BusRequestor fields
    requestor.operation.now = 0;  // Default operation
    requestor.operation.updated = 0;  // Default operation
    requestor.address.now = 0;    // Default address
    requestor.address.updated = 0;    // Default address
    requestor.busRequestorOccupied.now = false;
    requestor.busRequestorOccupied.updated = false;
    requestor.busRequestInTransaction.now = false;
    requestor.busRequestInTransaction.updated = false;
    requestor.priority.now = 0; // Default priority
    requestor.priority.updated = 0; // Default priority
    requestor.id = id; // Set the provided ID

    return requestor;
}

// Function to check if the BusRequestor is already occupied
bool busRequestorAlreadyOccupied(const BusRequestor* requestor) {
    return requestor->busRequestorOccupied.now;
}

// Function to reset the BusRequestor to its initial state
void resetBusRequestor(BusRequestor* requestor) {
    if (!requestor) {
        return; // Return if the pointer is null
    }

    // Reset all fields to their default state
    requestor->operation.updated                = 0;        // Reset operation
    requestor->address.updated                  = 0;        // Reset address
    requestor->busRequestorOccupied.now         = false;    // The now field. We want to allow
                                                            // a receive of requests in
                                                            // the last cycle of some transaction.
    requestor->busRequestInTransaction.updated  = false;
    requestor->priority.updated                 = 0;        // Default priority
}

// Function to request an action from the bus (sets the request operation and address)
void RequestActionFromBus(BusRequestor* requestor, int addr, int BusActionType) {
    // Check if the requestor is free (not occupied)
    if (!requestor->busRequestorOccupied.now)
    {
        // Mark it now! i.e. in the now fields.
        // The updated field shall be computed later by whether the next cycle shall be
        // the last cycle needed to finish the requested request.
        // For example, a busRd command takes only one cycle.

        // Set the operation type and address if the requestor is free
        requestor->operation.now    =   BusActionType;
        requestor->address.now      =   addr;

        // Mark the requestor as occupied
        requestor->busRequestorOccupied.now = true;
    }
}

// Function to perform the bus operation if granted
void DoOperation(BusRequestor* requestor, BusManager* manager, int cycle) {
    // Check if the requestor is occupied (if not, return)
    if (!requestor->busRequestorOccupied.now) {
        return;
    }

    // Did we already start the transaction?
    if (requestor->busRequestInTransaction.now)
    {
        // These instructions take only one cycle.
        // Therefore in the next cycle, we can reset the requestor
        if (requestor->operation.now == BusRd ||
            requestor->operation.now == BusRdX)
        {
            
        }
    }


    // Check if the requestor is not currently in transaction and if the bus manager grants the request
    if (!requestor->busRequestInTransaction.now && manager->RequestForBusGrantedRightNow()) {
        // Set the requestor's transaction as in progress
        requestor->busRequestInTransaction.updated = true;

        // Set the manager's bus details based on the requestor's request
        manager->bus_origid = requestor->id;
        manager->bus_cmd = requestor->operation;
        manager->bus_addr = requestor->address;
    }
}
