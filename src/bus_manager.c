#include "limits.h"
#include "bus_manager.h"
#include "core.h"


void configure_bus_manager(BusManager* manager, struct core** cores, MainMemory* main_memory)
{
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
    for (int i = 0; i < NUM_OF_CORES; ++i) {
        manager->enlisted_requestors[i] = NULL;
    }

    // Assign the provided requestors
    for (int i = 0; i < NUM_OF_CORES; ++i) {
        manager->requestors[i] = &(cores[i]->requestor);
    }

    // Assign the provided cores
    for (int i = 0; i < NUM_OF_CORES; ++i) {
        manager->cores[i] = cores[i];
    }
}


// Function to reset the enlisted_requestors list to zero
void resetEnlistedRequestors(BusManager* manager) {
    if (!manager) {
        fprintf(stderr, "Failed to get manager.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < NUM_OF_CORES; ++i) {
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
    for (int i = 0; i < NUM_OF_CORES; ++i) {
        if (manager->requestors[i]) { // Ensure pointer is valid before resetting
            bus_requestor_reset(manager->requestors[i]);
        }
    }

    resetEnlistedRequestors(manager);
}

// Function to arrange the priorities of the requestors based on the bus_origid
void arrangePriorities(BusManager* manager) {
    if (!manager || manager->bus_origid.updated < 0 || 
        manager->bus_origid.updated >= NUM_OF_CORES)
    {
        printf("error in artangepriorities inputs");
        exit(EXIT_FAILURE);
    }

    int originIndex = manager->bus_origid.updated;
    int originPriority = manager->requestors[originIndex]->priority;

    for (int i = 0; i < NUM_OF_CORES; ++i) {
        if (i != originIndex &&
            manager->requestors[i]->priority > originPriority)
        {
            manager->requestors[i]->priority -= 1;
        }
    }
    manager->requestors[originIndex]->priority = NUM_OF_CORES;
}

// Function to write to the bus line after a requestor is chosen
void WriteBusLines(BusManager* manager, BusRequestor* requestor)
{
    manager->bus_cmd.updated = requestor->operation;
    manager->bus_addr.updated = requestor->address;
    manager->bus_origid.updated = requestor->myCore->id;
}

// Function to assign the core with the lowest priority to core_turn
void finish_bus_enlisting(BusManager* manager)
{
    if (!manager) {
        return;
    }

    int minPriority = INT_MAX;
    int coreWithMinPriority = -1;

    for (int i = 0; i < NUM_OF_CORES; ++i) {
        if (manager->enlisted_requestors[i] != NULL) {
            if (manager->enlisted_requestors[i]->myCore->id != 0)
            {
                int currentPriority = manager->enlisted_requestors[i]->priority;
                if (currentPriority < minPriority)
                {
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

// Function to get the state that needed to be put in the cache
uint32_t StateToUpdate(BusManager* manager)
{
    if (manager->bus_shared.now) {
        return SHARED;
    }
    return EXCLUSIVE;
}

void write_next_cycle_of_bus(BusManager* manager)
{
    if (!manager)
    {
        fprintf(stderr, "Failed to no manager.\n");
        exit(EXIT_FAILURE);
    }

    // Reset the enlisted requestors array
    resetEnlistedRequestors(manager);

    switch (manager->BusStatus.now)
    {
        case BUS_FREE:
        {
            // Change status only if core_turn is valid
            if (manager->core_turn != -1) {
                manager->BusStatus.updated = manager->bus_cmd.updated; // Transition to the current bus command
                manager->requestors[manager->core_turn]->IsRequestOnBus.updated = true;
            }
            break;
        }
        case BUS_RD:
        case BUS_RDX:
        {
            // Transition to BUS_BEFORE_FLUSH
            manager->BusStatus.updated = BUS_BEFORE_FLUSH;
            manager->LastTransactionCycle = *(manager->p_cycle);
            // If memory stage only asked for busrdx without response update last cycle.
            if (get_state(manager->bus_addr.now,
                        &(manager->cores[manager->bus_origid.now]->cache_now)) == SHARED &&
                manager->bus_cmd.now == BUS_RDX) {
                manager->requestors[manager->bus_origid.now]->LastCycle.updated = true;
                bus_manager_reset(manager);
                
            }
            break;
        }
        case BUS_BEFORE_FLUSH:
        {
            // check if a cache interupted
            if (manager->Interupted.now) {
                manager->BusStatus.updated = BUS_CACHE_INTERRUPTED;
                break;
            }
            // Transition to BUS_FLUSH if 15 cycles have passed
            if (*(manager->p_cycle) - manager->LastTransactionCycle >= 15) {
                manager->BusStatus.updated = BUS_FLUSH;
                manager->bus_cmd.updated = BUS_FLUSH;
            }
            break;
        }
        case BUS_FLUSH: 
        {
            writeBusData(manager,
                         true,
                         &(manager->cores[manager->bus_origid.now]->cache_updated));
            // Increment the number of cycles in the same status
            manager->numOfCyclesInSameStatus++;
            // Transition to BUS_FREE if the block size limit is reached
            if (manager->numOfCyclesInSameStatus == BLOCK_SIZE) {
                main_memory_read(manager->main_memory,
                                &(manager->cores[manager->bus_origid.now]->cache_updated),
                                manager->bus_addr.now, StateToUpdate(manager)) ;
                manager->requestors[manager->bus_origid.now]->LastCycle.updated = true;
                bus_manager_reset(manager);
            }
            break;
        }
        case BUS_CACHE_INTERRUPTED:
        {
            if(manager->requestors[manager->bus_origid.now]->operation == BUS_RD) {
                writeBusData(manager,
                             false,
                             &(manager->cores[manager->bus_origid.now]->cache_now));
                // Increment the number of cycles in the same status
                manager->numOfCyclesInSameStatus++;
                if (manager->numOfCyclesInSameStatus == BLOCK_SIZE) {
                    main_memory_write(manager->main_memory,
                                      &(manager->cores[manager->interuptor_id]->cache_now),
                                      manager->bus_addr.now);
                    main_memory_read(manager->main_memory, 
                                     &(manager->cores[manager->bus_origid.now]->cache_updated),
                                     manager->bus_addr.now, StateToUpdate(manager));
                    manager->requestors[manager->bus_origid.now]->LastCycle.updated = true;
                    bus_manager_reset(manager);
                }
            }
            break;
        }
        default:
        {
            break;
        }
    }
}

// Function to request an action from the bus (sets the request operation and address)
void Enlist(BusRequestor* requestor, int addr, int BusActionType, BusManager* manager)
{
    // Set the operation type and address if the requestor is free
    requestor->operation = BusActionType;
    requestor->address = addr;

    // Mark the requestor as occupied
    manager->enlisted_requestors[requestor->myCore->id] = requestor;
}

void writeBusData(BusManager* manager, bool read, Cache* cache)
{
    uint32_t read_address;
    if (manager->bus_line_addr.now == 0)
    {
        read_address = manager->bus_addr.now - get_block_offset(manager->bus_addr.now);
    }
    else
    {
        read_address = manager->bus_line_addr.now + 1;
    }

    if (read)
    {
        manager->bus_data.updated = manager->main_memory->memory[read_address];
        manager->bus_line_addr.updated = read_address;
        return;
    }

    if (!read)
    {
        manager->bus_data.updated = read_cache(read_address, cache);
        manager->bus_line_addr.updated = read_address;
        return;
    }
}

// function to call when bus needs to be interupted (state m and busrd/busrdx)
void InterruptBus(BusManager* manager, int32_t interruptor_id)
{
    manager->Interupted.updated = true;
    manager->interuptor_id = interruptor_id;
}

// Function to advance the bus to the next cycle
void advance_bus_to_next_cycle(BusManager* manager)
{
    UPDATE_FLIP_FLOP(manager->BusStatus, false);
    UPDATE_FLIP_FLOP(manager->Interupted, false);
    UPDATE_FLIP_FLOP(manager->bus_shared, false);
    UPDATE_FLIP_FLOP(manager->bus_cmd, false);
    UPDATE_FLIP_FLOP(manager->bus_origid, false);
    UPDATE_FLIP_FLOP(manager->bus_addr, false);
    UPDATE_FLIP_FLOP(manager->bus_line_addr, false);
    UPDATE_FLIP_FLOP(manager->bus_data, false);
}
