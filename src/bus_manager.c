#include "bus_manager.h"


BusManager* bus_manager_create(BusRequestor** requestors, Core** cores, MainMemory* main_memory) {
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
    manager->bus_shared.now = false;
    manager->bus_shared.updated = false;
    manager->bus_origid.now = -1;
    manager->bus_origid.updated = -1;
    manager->bus_cmd.now = 0;
    manager->bus_cmd.updated = 0;
    manager->bus_addr.now = 0;
    manager->bus_addr.updated = 0;
    manager->bus_line_addr.now = 0;
    manager->bus_line_addr.updated = 0;
    manager->bus_data.now = 0;
    manager->bus_data.updated = 0;
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

    // Assign the provided cores
    for (int i = 0; i < NUM_REQUESTORS; ++i) {
        manager->cores[i] = cores[i];
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
    manager->bus_shared.now = false;
    manager->bus_shared.updated = false;
    manager->bus_origid.now = -1;
    manager->bus_origid.updated = -1;
    manager->bus_cmd.now = 0;
    manager->bus_cmd.updated = 0;
    manager->bus_addr.now = 0;
    manager->bus_addr.updated = 0;
    manager->bus_line_addr.now = 0;
    manager->bus_line_addr.updated = 0;
    manager->bus_data.now = 0;
    manager->bus_data.updated = 0;
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
    if (!manager || manager->bus_origid.updated < 0 || manager->bus_origid.updated >= NUM_REQUESTORS) {
        printf("error in artangepriorities inputs");
        exit(EXIT_FAILURE);
    }

    int originIndex = manager->bus_origid.updated;
    int originPriority = manager->requestors[originIndex]->priority;

    for (int i = 0; i < NUM_REQUESTORS; ++i) {
        if (i != originIndex && manager->requestors[i]->priority > originPriority) {
            manager->requestors[i]->priority -= 1;
        }
    }
    manager->requestors[originIndex]->priority = NUM_REQUESTORS;
}

// Function to assign the core with the lowest priority to core_turn
void FinishBusEnlisting(BusManager* manager) {
    if (!manager) {
        return;
    }

    int minPriority = INT_MAX;
    int coreWithMinPriority = -1;

    for (int i = 0; i < NUM_REQUESTORS; ++i) {
        if (manager->enlisted_requestors[i] != NULL) {
            if (manager->enlisted_requestors[i]->id != 0) {
                int currentPriority = manager->enlisted_requestors[i]->priority;
                if (currentPriority < minPriority) {
                    minPriority = currentPriority;
                    coreWithMinPriority = i;
                }
            }
        }
        else
        {
            continue;
        }
    }

    manager->core_turn = coreWithMinPriority;
    WriteBusLines(manager, manager->requestors[coreWithMinPriority]);
    arrangePriorities(manager);        

}

// Function to request an action from the bus (sets the request operation and address)
void Enlist(BusRequestor* requestor, int addr, int BusActionType, BusManager* manager) {
    // Set the operation type and address if the requestor is free
    requestor->operation = BusActionType;
    requestor->address = addr;

    // Mark the requestor as occupied
    manager->enlisted_requestors[requestor->id] = requestor;
}

void writeBusData(BusManager* manager, int32_t diff, bool read, Cache* cache) {
    uint32_t read_address;
    if (manager->bus_line_addr.now == 0) {
        read_address = manager->bus_addr.now - get_block_offset(manager->bus_addr.now);
    }
    else {
        read_address = manager->bus_line_addr.now + 1;
    }

    if (read) {
        manager->bus_data.updated = manager->main_memory->memory[read_address];
        manager->bus_line_addr.updated = read_address;
        return;
    }

    if (!read) {
        manager->bus_data.updated = read_cache(read_address, cache);
        manager->bus_line_addr.updated = read_address;
        return;
    }
}

void WriteBusLines(BusManager* manager, BusRequestor* requestor) {
    manager->bus_cmd.updated = requestor->operation;
    manager->bus_addr.updated = requestor->address;
    manager->bus_origid.updated = requestor->id;
}

// function to call when bus needs to be interupted (state m and busrd/busrdx)
void InterruptBus(BusManager* manager, int32_t interruptor_id) {
    manager->Interupted.updated = true;
    manager->interuptor_id = interruptor_id;
}

// Function to get the state that needed to be put in the cache
uint32_t StateToUpdate(BusManager* manager) {
    if (manager->bus_shared.now) {
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
    case BUS_FREE:
        // Change status only if core_turn is valid
        if (manager->core_turn != -1) {
            manager->BusStatus.updated = manager->bus_cmd.updated; // Transition to the current bus command
            manager->requestors[manager->core_turn]->IsRequestOnBus.updated = true;
        }
        break;

    case BUS_RD:
    case BUS_RDX:
        // Transition to BUS_BEFORE_FLUSH
        manager->BusStatus.updated = BUS_BEFORE_FLUSH;
        manager->LastTransactionCycle = currentCycle;
        // If memory stage only asked for busrdx without response update last cycle.
        if (get_state(manager->bus_addr.now, manager->cores[manager->bus_origid.now]->cache_now) == SHARED && manager->bus_cmd.now == BUS_RDX) {
            manager->requestors[manager->bus_origid.now]->LastCycle.updated = true;
            bus_manager_reset(manager);
            
        }
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
            manager->bus_cmd.updated = BUS_FLUSH;
        }
        break;

    case BUS_FLUSH: 
        
        writeBusData(manager, diff, true, manager->cores[manager->bus_origid.now]->cache_updated);
        // Increment the number of cycles in the same status
        manager->numOfCyclesInSameStatus++;
        // Transition to BUS_FREE if the block size limit is reached
        if (manager->numOfCyclesInSameStatus == BLOCK_SIZE) {
            main_memory_read(manager->main_memory, manager->cores[manager->bus_origid.now]->cache_updated, manager->bus_addr.now, StateToUpdate(manager)) ;
            manager->requestors[manager->bus_origid.now]->LastCycle.updated = true;
            bus_manager_reset(manager);
        }
        break;

    case BUS_CACHE_INTERRUPTED:
        if(manager->requestors[manager->bus_origid.now]->operation == BUS_RD) {
            writeBusData(manager, diff, false, manager->cores[manager->bus_origid.now]->cache_now);
            // Increment the number of cycles in the same status
            manager->numOfCyclesInSameStatus++;
            if (manager->numOfCyclesInSameStatus == BLOCK_SIZE) {
                main_memory_write(manager->main_memory, manager->cores[manager->interuptor_id]->cache_now, manager->bus_addr.now);
                main_memory_read(manager->main_memory, manager->cores[manager->bus_origid.now]->cache_updated, manager->bus_addr.now, StateToUpdate(manager));
                manager->requestors[manager->bus_origid.now]->LastCycle.updated = true;
                bus_manager_reset(manager);
            }
            break;
        }
        //  this is unnecessery becasue the behiviour is the same i think for rd and rdx
        //else {          // cmd is bus_rdx , this is un
            //// XXX: Do a validate for the bus rdx
            //writeBusData(manager, diff, false, manager->caches[manager->bus_origid.now]);    // Write the cache response to main memory
            //writeBusData(manager, diff, true, manager->caches[manager->bus_origid.now]);    // Write cache response to the cache requestor
            //// XXX: Update numbers. and insert first if logic to thired if.
            //// XXX: total update here. need rerwriting.
            //if (currentCycle - manager->LastTransactionCycle == 39) {
            //    manager->requestors[manager->bus_origid.now]->LastCycle.updated = true;
            //}
            //if ((currentCycle - manager->LastTransactionCycle == 20)) {
            //    main_memory_write(manager->main_memory, manager->caches[manager->interuptor_id], manager->bus_addr.now);
            //}
            //if (currentCycle - manager->LastTransactionCycle == 40) {
            //    main_memory_read(manager->main_memory, manager->caches[manager->bus_origid.now], manager->bus_addrnow, StateToUpdate(manager, manager->caches[manager->bus_origid.now]));
            //    bus_manager_reset(manager);
            //}
            //break;
        //}

    default:
        break;
    }
    UPDATE_FLIP_FLOP(manager->BusStatus, KeepValue);
    UPDATE_FLIP_FLOP(manager->Interupted, KeepValue);
    UPDATE_FLIP_FLOP(manager->bus_shared, KeepValue);
    UPDATE_FLIP_FLOP(manager->bus_cmd, KeepValue);
    UPDATE_FLIP_FLOP(manager->bus_origid, KeepValue);
    UPDATE_FLIP_FLOP(manager->bus_addr, KeepValue);
    UPDATE_FLIP_FLOP(manager->bus_line_addr, KeepValue);
    UPDATE_FLIP_FLOP(manager->bus_data, KeepValue);

}

// Function for the BusSnooper to snoop and react to bus commands
void snoop(BusSnooper* snooper, Cache* cache, BusManager* manager) {
    if (!snooper || !cache || !manager) {
        return; // Ensure all pointers are valid
    }

    // Check if the bus command is BUS_RD or BUS_RDX and the originating ID isn't the snooper's ID
    if ((manager->bus_cmd.now == BUS_RD || manager->bus_cmd.now == BUS_RDX) && manager->bus_origid.now != snooper->id) {

        uint32_t address = manager->bus_addr.now;
        int state = get_state(address, cache);
        if (in_cache(address, cache)) {

            // If the cache line is MODIFIED and the command is BUS_RD
            if (state == MODIFIED && manager->bus_cmd.now == BUS_RD) {
                manager->interuptor_id = snooper->id;
                manager->Interupted.updated = true;
                update_state(address, cache, Shared);
                return;
            }

            // If the cache line is modified and the command is bus rdx, interupt bus, and change state to invalid
            if (state == MODIFIED && manager->bus_cmd.now == BUS_RDX) {
                manager->interuptor_id = snooper->id;
                manager->Interupted.updated = true;
                update_state(address, cache, INVALID);
                return;
            }

            // If the command is BUS_RDX and the address is in cache, invalidate the cache line
            if (manager->bus_cmd.now == BUS_RDX) {
                update_state(address, cache, INVALID);
            }

            // If the command is BUS_RD and the cache state is EXCLUSIVE or SHARED, update to SHARED
            if (manager->bus_cmd.now == BUS_RD) {
                update_state(address, cache, SHARED);
                manager->bus_shared.updated = true;
            }
        }

    }
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
