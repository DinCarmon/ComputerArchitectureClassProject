#include "bus_manager.h"

// Function to create and initialize a BusManager
BusManager bus_manager_create(BusRequestor requestors[NUM_REQUESTORS]) {
    BusManager manager;

    // Initialize bus attributes
    manager.LastTransactionCycle = 0;
    manager.BusStatus = BUS_FREE;
    manager.bus_shared = false;
    manager.bus_origid = -1;
    manager.bus_cmd = 0;
    manager.bus_addr = 0;
    manager.bus_data = 0;
    manager.core_turn = 0;
    manager.numOfCyclesInSameStatus = 0;

    // Initialize enlisted_requestors list as empty
    for (int i = 0; i < NUM_REQUESTORS; ++i) {
        manager.enlisted_requestors[i] = (BusRequestor){ 0 };
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
    manager->BusStatus = BUS_FREE;
    manager->bus_shared = false;
    manager->bus_origid = -1;
    manager->bus_cmd = 0;
    manager->bus_addr = 0;
    manager->bus_data = 0;
    manager->numOfCyclesInSameStatus = 0;

    // Reset each BusRequestor
    for (int i = 0; i < NUM_REQUESTORS; ++i) {
        resetBusRequestor(&manager->requestors[i]);
    }

    // Reset enlisted_requestors list and core_turn
    resetEnlistedRequestors(manager);
}

// Function to check if the bus is free
bool IsBusFree(const BusManager* manager) {
    return manager && manager->BusStatus == BUS_FREE;
}

// Function to arrange the priorities of the requestors based on the bus_origid
void arrangePriorities(BusManager* manager) {
    if (!manager) {
        return;
    }

    int originIndex = manager->bus_origid;
    if (originIndex < 0 || originIndex >= NUM_REQUESTORS) {
        return;
    }

    int originPriority = manager->requestors[originIndex].priority;
    for (int i = 0; i < NUM_REQUESTORS; ++i) {
        if (i != originIndex && manager->requestors[i].priority > originPriority) {
            manager->requestors[i].priority -= 1;
        }
    }
    manager->requestors[originIndex].priority = NUM_REQUESTORS;
}

// Function to add a BusRequestor to the enlisted_requestors list
void RequestForBus(BusManager* manager, BusRequestor requestor) {
    if (!manager) {
        return;
    }

    for (int i = 0; i < NUM_REQUESTORS; ++i) {
        if (manager->enlisted_requestors[i].id == 0) {
            manager->enlisted_requestors[i] = requestor;
            break;
        }
    }
}

// Function to reset the enlisted_requestors list to zero
void resetEnlistedRequestors(BusManager* manager) {
    if (!manager) {
        return;
    }

    for (int i = 0; i < NUM_REQUESTORS; ++i) {
        manager->enlisted_requestors[i] = (BusRequestor){ 0 };
    }

    manager->core_turn = 0;
}

// Function to assign the core with the lowest priority to core_turn
void FinishBusEnlisting(BusManager* manager) {
    if (!manager) {
        return;
    }

    int minPriority = INT_MAX;
    int coreWithMinPriority = -1;

    for (int i = 0; i < NUM_REQUESTORS; ++i) {
        if (manager->enlisted_requestors[i].id != 0) {
            int currentPriority = manager->enlisted_requestors[i].priority;
            if (currentPriority < minPriority) {
                minPriority = currentPriority;
                coreWithMinPriority = i;
            }
        }
    }

    if (coreWithMinPriority != -1) {
        manager->core_turn = coreWithMinPriority;
    }
}

// Function to advance the bus to the next cycle
void AdvanceBusToNextCycle(BusManager* manager, int currentCycle) {
    if (!manager) {
        return; // Handle null pointer
    }

    // Reset the enlisted requestors array
    resetEnlistedRequestors(manager);

    switch (manager->BusStatus) {
    case BUS_FREE:
        // Change status only if core_turn is valid
        if (manager->core_turn != -1) {
            manager->BusStatus = manager->bus_cmd; // Transition to the current bus command
        }
        break;

    case BUS_RD:
    case BUS_RDX:
        // Transition to BUS_BEFORE_FLUSH
        manager->BusStatus = BUS_BEFORE_FLUSH;
        manager->LastTransactionCycle = currentCycle;
        break;

    case BUS_BEFORE_FLUSH:
        // Transition to BUS_FLUSH if 15 cycles have passed
        if (currentCycle - manager->LastTransactionCycle >= 15) {
            manager->BusStatus = BUS_FLUSH;
            manager->core_turn = -1; // Set core_turn to -1
        }
        break;

    case BUS_FLUSH:
        // Increment the number of cycles in the same status
        manager->numOfCyclesInSameStatus++;
        // Transition to BUS_FREE if the block size limit is reached
        if (manager->numOfCyclesInSameStatus == BLOCK_SIZE) {
            manager->BusStatus = BUS_FREE;
            manager->numOfCyclesInSameStatus = 0; // Reset the counter
        }
        break;

    default:
        break;
    }
}
