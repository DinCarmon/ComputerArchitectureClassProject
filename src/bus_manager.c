#include "bus_manager.h"

// Function to create and initialize a BusManager
BusManager bus_manager_create(BusRequestor requestors[NUM_REQUESTORS]) {
    BusManager manager;

    // Initialize bus attributes
    manager.LastTransactionCycle = 0;
    manager.BusStatus = 0; // Idle
    manager.bus_shared = false;
    manager.bus_origid = -1;
    manager.bus_cmd = 0;
    manager.bus_addr = 0;
    manager.bus_data = 0;
    manager.core_turn = 0;  // Initialize core_turn to 0

    // Initialize enlisted_requestors list as empty (i.e., NULL)
    for (int i = 0; i < NUM_REQUESTORS; ++i) {
        manager.enlisted_requestors[i] = (BusRequestor){ 0 };  // Setting all to zero (empty)
    }

    // Copy the provided requestors into the manager's array
    for (int i = 0; i < NUM_REQUESTORS; ++i) {
        manager.requestors[i] = requestors[i];
    }

    return manager;
}

// Function to reset the BusManager after a transaction
void bus_manager_reset(BusManager* manager) {
    if (!manager) {
        return;
    }

    // Reset bus attributes
    manager->BusStatus = 0;
    manager->bus_shared = false;
    manager->bus_origid = -1;
    manager->bus_cmd = 0;
    manager->bus_addr = 0;
    manager->bus_data = 0;

    // Reset each BusRequestor
    for (int i = 0; i < NUM_REQUESTORS; ++i) {
        manager->requestors[i].busRequestorOccupied_now = false;
        manager->requestors[i].busRequestorOccupied_updated = false;
        manager->requestors[i].busRequestInTransaction_now = false;
        manager->requestors[i].request.operation = 0;
        manager->requestors[i].request.address = 0;
        manager->requestors[i].request.start_cycle = 0;
    }

    // Reset enlisted_requestors list and core_turn
    resetEnlistedRequestors(manager);
}

// Function to check if the bus is free
bool IsBusFree(const BusManager* manager) {
    if (!manager) {
        return false; // Assume bus is not free if manager is NULL
    }
    return manager->BusStatus == 0;
}

// Function to arrange the priorities of the requestors based on the bus_origid
void arrangePriorities(BusManager* manager) {
    if (!manager) {
        return; // Handle null manager pointer
    }

    // Get the originator's index (bus_origid) in the requestors array
    int originIndex = manager->bus_origid;

    // Ensure bus_origid is valid
    if (originIndex < 0 || originIndex >= NUM_REQUESTORS) {
        return;
    }

    // Get the priority of the requestor at the bus_origid index
    int originPriority = manager->requestors[originIndex].priority;

    // Iterate through the requestors and adjust priorities
    for (int i = 0; i < NUM_REQUESTORS; ++i) {
        if (i != originIndex) {
            // If requestor's priority is higher, subtract 1 from it
            if (manager->requestors[i].priority > originPriority) {
                manager->requestors[i].priority -= 1;
            }
        }
    }

    // Set the priority of the requestor at bus_origid to the number of cores
    manager->requestors[originIndex].priority = NUM_REQUESTORS;
}

// Function to add a BusRequestor to the enlisted_requestors list
void RequestForBus(BusManager* manager, BusRequestor requestor) {
    if (!manager) {
        return; // Handle null manager pointer
    }

    // Try to find an empty spot in the enlisted_requestors array
    for (int i = 0; i < NUM_REQUESTORS; ++i) {
        if (manager->enlisted_requestors[i].id == 0) { // Assuming id == 0 means uninitialized/empty
            // Add the requestor to the list
            manager->enlisted_requestors[i] = requestor;
            break; // Exit the loop after adding the requestor
        }
    }
}

// Function to reset the enlisted_requestors list to zero
void resetEnlistedRequestors(BusManager* manager) {
    if (!manager) {
        return; // Handle null manager pointer
    }

    // Set all enlisted requestors to empty (zero) values
    for (int i = 0; i < NUM_REQUESTORS; ++i) {
        manager->enlisted_requestors[i] = (BusRequestor){ 0 };  // Setting all to zero (empty)
    }

    // Set core_turn to 0
    manager->core_turn = 0;
}

// Function to assign the core with the lowest priority to core_turn
void FinishBusEnlisting(BusManager* manager) {
    if (!manager) {
        return; // Handle null manager pointer
    }

    // Initialize the minimum priority to a large number
    int minPriority = INT_MAX;
    int coreWithMinPriority = -1;

    // Iterate through the enlisted_requestors list to find the core with the lowest priority
    for (int i = 0; i < NUM_REQUESTORS; ++i) {
        if (manager->enlisted_requestors[i].id != 0) { // Only consider valid enlisted requestors
            int currentPriority = manager->enlisted_requestors[i].priority;
            if (currentPriority < minPriority) {
                minPriority = currentPriority;
                coreWithMinPriority = i;
            }
        }
    }

    // Assign the core with the lowest priority to core_turn
    if (coreWithMinPriority != -1) {
        manager->core_turn = coreWithMinPriority;
    }
}
