#include "bus_manager.h"


BusManager* bus_manager_create(BusRequestor** requestors, Cache** caches, MainMemory* main_memory) {
    BusManager* manager = (BusManager*)malloc(sizeof(BusManager));
    if (!manager) {
        fprintf(stderr, "Failed to allocate memory for BusManager.\n");
        exit(EXIT_FAILURE);
    }

    // Initialize bus attributes
    manager->LastTransactionCycle = 0;
    manager->BusStatus.now = BUS_FREE;
    manager->BusStatus.updated = BUS_FREE;
    manager->Interupted.now = false;
    manager->Interupted.updated = false;
    manager->bus_shared = false;
    manager->bus_origid = -1;
    manager->bus_cmd = 0;
    manager->bus_addr = 0;
    manager->bus_data = 0;
    manager->core_turn = -1;
    manager->interuptor_id = -1;
    manager->numOfCyclesInSameStatus = 0;
    manager->main_memory = main_memory;

    // Initialize enlisted_requestors list as empty
    for (int i = 0; i < NUM_REQUESTORS; ++i) {
        manager->enlisted_requestors[i] = NULL;
    }

    // Assign the provided requestors
    for (int i = 0; i < NUM_REQUESTORS; ++i) {
        manager->requestors[i] = requestors[i];
    }

    // Assign the provided caches
    for (int i = 0; i < NUM_REQUESTORS; ++i) {
        manager->caches[i] = caches[i];
    }

    return manager;
}


// Reset enlisted requestors
void resetEnlistedRequestors(BusManager* manager) {
    if (!manager) {
        fprintf(stderr, "Failed to get manager.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < NUM_REQUESTORS; ++i) {
        manager->enlisted_requestors[i] = NULL;
    }

}

// Function to reset the BusManager
void bus_manager_reset(BusManager* manager) {
    if (!manager) {
        return;
    }

    manager->BusStatus.now = BUS_FREE;
    manager->BusStatus.updated = BUS_FREE;
    manager->Interupted.now = false;
    manager->Interupted.updated = false;
    manager->bus_shared = false;
    manager->bus_origid = -1;
    manager->bus_cmd = 0;
    manager->bus_addr = 0;
    manager->bus_data = 0;
    manager->numOfCyclesInSameStatus = 0;
    manager->core_turn = -1;
    manager->interuptor_id = -1;

    // Reset each BusRequestor
    for (int i = 0; i < NUM_REQUESTORS; ++i) {
        if (manager->requestors[i]) { // Ensure pointer is valid before resetting
            bus_requestor_reset(manager->requestors[i]);
        }
    }

    resetEnlistedRequestors(manager);
}

// Function to destroy a BusManager
void bus_manager_destroy(BusManager* manager) {
    if (manager) {
        free(manager);
    }
}

// Function to check if the bus is free
bool IsBusFree(const BusManager* manager) {
    return manager && manager->BusStatus.now == BUS_FREE;
}


// Function to arrange the priorities of the requestors based on the bus_origid
void arrangePriorities(BusManager* manager) {
    if (!manager || manager->bus_origid < 0 || manager->bus_origid >= NUM_REQUESTORS) {
        // XXX: exit is better.
        return;
    }

    // XXX: should use bus origid updated when it is changed to flip flop
    int originIndex = manager->bus_origid;
    int originPriority = manager->requestors[originIndex]->priority;

    for (int i = 0; i < NUM_REQUESTORS; ++i) {
        if (i != originIndex && manager->requestors[i]->priority > originPriority) {
            manager->requestors[i]->priority -= 1;
        }
    }
    manager->requestors[originIndex]->priority = NUM_REQUESTORS;
}


// Function to add a BusRequestor to the enlisted_requestors list
void RequestForBus(BusManager* manager, BusRequestor* requestor) {
    if (!manager || !requestor) {
        return;
    }

    for (int i = 0; i < NUM_REQUESTORS; ++i) {
        if (manager->enlisted_requestors[i] == NULL) {
            manager->enlisted_requestors[i] = requestor;
            break;
        }
    }
}


// Function to assign the core with the lowest priority to core_turn
void FinishBusEnlisting(BusManager* manager) {
    if (!manager) {
        return;
    }

    int minPriority = INT_MAX;
    int coreWithMinPriority = -1;

    for (int i = 0; i < NUM_REQUESTORS; ++i) {
        if (manager->enlisted_requestors[i]->id != 0) {
            int currentPriority = manager->enlisted_requestors[i]->priority;
            if (currentPriority < minPriority) {
                minPriority = currentPriority;
                coreWithMinPriority = i;
            }
        }
    }

    manager->core_turn = coreWithMinPriority;
    WriteBusLines(manager, manager->requestors[coreWithMinPriority]);
    arrangePriorities(manager);        

}

void writeBusData(BusManager* manager, int32_t diff, bool read, Cache* cache) {
    // XXX: update numbers
    int32_t cycle = diff - 17;
    int32_t read_address = manager->bus_addr - get_block_offset(manager->bus_addr) + cycle;
    if (diff >= 37 && read) {
        manager->bus_data = manager->main_memory->memory[read_address - 20];
        return;
    }

    if (diff >= 17 && read) {
        manager->bus_data = manager->main_memory->memory[read_address];
        return;
    }

    if (diff >= 17 && !read) {
        manager->bus_data = read_cache(read_address, cache);
        return;
    }
}

// XXX: update to flip flops
void WriteBusLines(BusManager* manager, BusRequestor* requestor) {
    manager->bus_cmd = requestor->operation;
    manager->bus_addr = requestor->address;
    manager->bus_origid = requestor->id;
}

// function to call when bus needs to be interupted (state m and busrd/busrdx)
void InterruptBus(BusManager* manager, int32_t interruptor_id) {
    manager->Interupted.updated = true;
    manager->interuptor_id = interruptor_id;
}

// Function to get the state that needed to be put in the cache
uint32_t StateToUpdate(BusManager* manager, Cache* cache) {
    if (manager->bus_shared) {
        return SHARED;
    }
    return EXCLUSIVE;
}

// Function to advance the bus to the next cycle
void AdvanceBusToNextCycle(BusManager* manager, int currentCycle, bool KeepValue ) {
    if (!manager) {
        fprintf(stderr, "Failed to no manager.\n");
        exit(EXIT_FAILURE);
    }

    // Reset the enlisted requestors array
    resetEnlistedRequestors(manager);
    int32_t diff = currentCycle - manager->LastTransactionCycle;

    switch (manager->BusStatus.now) {
        // XXX: update bus status correctly. state machine states are not coherent with bus possible statuses.
    case BUS_FREE:

        FinishBusEnlisting(manager); // XXX: Can be removed?
        // Change status only if core_turn is valid
        if (manager->core_turn != -1) {
            manager->BusStatus.updated = manager->bus_cmd; // Transition to the current bus command
            manager->requestors[manager->core_turn]->IsRequestOnBus.updated = true;
        }
        break;

    case BUS_RD:
    case BUS_RDX:
        // Transition to BUS_BEFORE_FLUSH
        manager->BusStatus.updated = BUS_BEFORE_FLUSH;
        manager->LastTransactionCycle = currentCycle;
        // XXX: If memory stage only asked for busrdx without response update last cycle.
        break;

    case BUS_BEFORE_FLUSH:
        // check if a cache interupted
        if (manager->Interupted.now) {
            manager->BusStatus.updated = BUS_CACHE_INTERRUPTED;
            break;
        }
        // Transition to BUS_FLUSH if 15 cycles have passed
        if (currentCycle - manager->LastTransactionCycle >= 15) {
            manager->BusStatus.updated = BUS_FLUSH;
        }
        break;

    case BUS_FLUSH: // XXX: in other words, flush from main memory
        
        writeBusData(manager, diff, true, manager->caches[manager->bus_origid]);
        if (currentCycle - manager->LastTransactionCycle == 19) {
            manager->requestors[manager->bus_origid]->LastCycle.updated = true;
        }
        // Increment the number of cycles in the same status
        manager->numOfCyclesInSameStatus++;
        // Transition to BUS_FREE if the block size limit is reached
        if (manager->numOfCyclesInSameStatus == BLOCK_SIZE) {
            // XXX: Need to check edge case where core intiator send a busrdx only to update other caches.
            main_memory_read(manager->main_memory, manager->caches[manager->bus_origid], manager->bus_addr, StateToUpdate(manager, manager->caches[manager->bus_origid])) ;
            bus_manager_reset(manager);
        }
        break;

    case BUS_CACHE_INTERRUPTED:
        // XXX: There wont be a bus read on the bus. need to check the if according to the requestor.
        if(manager->bus_cmd == BUS_RD) {
            writeBusData(manager, diff, false, manager->caches[manager->bus_origid]);
            // XXX: update number 19 / 20
            if (currentCycle - manager->LastTransactionCycle == 19) {
                manager->requestors[manager->bus_origid]->LastCycle.updated = true;
            }
            if (currentCycle - manager->LastTransactionCycle >= 20) {
                main_memory_write(manager->main_memory, manager->caches[manager->interuptor_id], manager->bus_addr);
                main_memory_read(manager->main_memory, manager->caches[manager->bus_origid], manager->bus_addr, StateToUpdate(manager, manager->caches[manager->bus_origid]));

                bus_manager_reset(manager);
            }
            break;
        }
        else {          // cmd is bus_rdx
            // XXX: Do a validate for the bus rdx
            writeBusData(manager, diff, false, manager->caches[manager->bus_origid]);    // Write the cache response to main memory
            writeBusData(manager, diff, true, manager->caches[manager->bus_origid]);    // Write cache response to the cache requestor
            // XXX: Update numbers. and insert first if logic to thired if.
            // XXX: total update here. need rerwriting.
            if (currentCycle - manager->LastTransactionCycle == 39) {
                manager->requestors[manager->bus_origid]->LastCycle.updated = true;
            }
            if ((currentCycle - manager->LastTransactionCycle == 20)) {
                main_memory_write(manager->main_memory, manager->caches[manager->interuptor_id], manager->bus_addr);
            }
            if (currentCycle - manager->LastTransactionCycle == 40) {
                main_memory_read(manager->main_memory, manager->caches[manager->bus_origid], manager->bus_addr, StateToUpdate(manager, manager->caches[manager->bus_origid]));
                bus_manager_reset(manager);
            }
            break;
        }

    default:
        break;
    }
    UPDATE_FLIP_FLOP(manager->BusStatus, KeepValue);
    UPDATE_FLIP_FLOP(manager->Interupted, KeepValue);

}

// Function to do the bus requestor operation, the function assumes that there is a need to call the bus
// the function will return true if the operation is over and false if a stall is needed 
// XXX: Delete it. obsolete.
bool RequestorDoOperation(BusRequestor* requestor, BusManager* manager, uint32_t address, uint32_t cmd, int cycle) {
    bool BusFree = IsBusFree(manager);
    if (!requestor->IsRequestOnBus.now && BusFree) {
        manager->enlisted_requestors[requestor->id] = requestor;
        return false;
    }
    
    if (requestor->LastCycle.now) {
        return true;
    }
    return false;

}
