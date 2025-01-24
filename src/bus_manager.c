#include "limits.h"
#include "bus_manager.h"
#include "core.h"


void configure_bus_manager(BusManager* manager, Core cores[NUM_OF_CORES], MainMemory* main_memory)
{
    // Initialize bus attributes
    manager->last_transaction_on_bus_cycle = 0;
    manager->bus_status.now = BUS_FREE;
    manager->bus_status.updated = BUS_FREE;
    manager->interrupted_by_another_snooper = false;
    manager->bus_shared.now = BLOCK_NOT_SHARED;
    manager->bus_shared.updated = BLOCK_NOT_SHARED;
    manager->bus_origid.now = -1;
    manager->bus_origid.updated = -1;
    manager->bus_cmd.now = NO_CMD;
    manager->bus_cmd.updated = NO_CMD;
    manager->bus_line_addr.now = 0;
    manager->bus_line_addr.updated = 0;
    manager->bus_data.now = 0;
    manager->bus_data.updated = 0;
    manager->core_turn.now = -1;
    manager->core_turn.now = -1;
    manager->interuptor_id = -1;
    manager->main_memory = main_memory;

    // Initialize enlisted_requestors list as empty
    for (int i = 0; i < NUM_OF_CORES; ++i) {
        manager->enlisted_requestors[i] = NULL;
    }

    // Assign the provided requestors
    for (int i = 0; i < NUM_OF_CORES; ++i) {
        manager->requestors[i] = &(cores[i].requestor);
    }
}

// Function to reset the enlisted_requestors list to zero
void reset_enlisted_requestors(BusManager* manager) {
    if (!manager) {
        fprintf(stderr, "Failed to get manager.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < NUM_OF_CORES; ++i) {
        manager->enlisted_requestors[i] = NULL;
    }
}

// Function to arrange the priorities of the requestors based on the bus_origid
void arrange_priorities(BusManager* manager) {
    if (manager->core_turn.updated == -1) // i.e no one request to enlist and therefore there is no need to rearrange priorities
        return;

    int originIndex = manager->core_turn.updated;
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
void write_bus_lines_from_requestor(BusManager* manager, BusRequestor* requestor)
{
    manager->bus_cmd.updated = requestor->operation;
    manager->bus_line_addr.updated = requestor->address;
    manager->bus_origid.updated = requestor->myCore->id;
    manager->bus_shared.updated = BLOCK_NOT_SHARED;
    if (requestor->operation != FLUSH_CMD)
        manager->bus_data.updated = 0;
    else
        manager->bus_data.updated = requestor->myCore->cache_now.dsram[memory_address_to_cache_address(requestor->address - get_block_offset(requestor->address))];
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
        if (manager->enlisted_requestors[i] != NULL)
        {
            int currentPriority = manager->enlisted_requestors[i]->priority;
            if (currentPriority < minPriority)
            {
                minPriority = currentPriority;
                coreWithMinPriority = i;
            }
        }
        else
        {
            continue;
        }
    }

    manager->core_turn.updated = coreWithMinPriority;
    arrange_priorities(manager);

    // Reset the enlisted requestors array
    reset_enlisted_requestors(manager);
}

// Must occur after bus enlisting and finish enlisting
void write_next_cycle_of_bus(BusManager* manager)
{
    if (!manager)
    {
        fprintf(stderr, "Failed to no manager.\n");
        exit(EXIT_FAILURE);
    }

    // First check if we are on the verge of a new transaction.
    // If so update accordingly
    if (manager->core_turn.updated != -1 &&
        is_open_for_start_of_new_enlisting(manager))
    {
        // Let the bus requestor enter the bus
        write_bus_lines_from_requestor(manager, manager->requestors[manager->core_turn.updated]);

        // Transition of the state to the current bus command
        if (manager->bus_cmd.updated == BUS_RD_CMD)
            manager->bus_status.updated = BUS_RD;
        else if (manager->bus_cmd.updated == BUS_RDX_CMD)
            manager->bus_status.updated = BUS_RDX;
        else if (manager->bus_cmd.updated == FLUSH_CMD)
            manager->bus_status.updated = BUS_FLUSH;
        else
        {
            // This is weird. How did a core enlisted but not requested anything. Should abort
            fprintf(stderr, "Enlisting without any request is not possible.\n");
            exit(EXIT_FAILURE);
        }
    }

    switch (manager->bus_status.now)
    {
        case BUS_FREE:
        {
            // I.E if the bus should stay free
            if (manager->core_turn.updated == -1)
            {
                manager->bus_status.updated = BUS_FREE;
                manager->bus_cmd.updated = NO_CMD;
                manager->bus_line_addr.updated = 0;
                manager->bus_data.updated = 0;
                manager->bus_origid.updated = 0;
                manager->bus_shared.updated = BLOCK_NOT_SHARED;
            }
            break;
        }
        case BUS_RD:
        case BUS_RDX:
        {
            // Transition to BUS_BEFORE_FLUSH
            manager->bus_status.updated = BUS_BEFORE_FLUSH;
            manager->bus_cmd.updated = NO_CMD;
            manager->bus_line_addr.updated = 0;
            manager->bus_data.updated = 0;
            manager->bus_origid.updated = 0;

            // Check if another cache wishes to answer, and answered in the snooping on this cycle.
            if (manager->interrupted_by_another_snooper)
            {
                manager->bus_status.updated = BUS_CACHE_INTERRUPTED;
                manager->bus_cmd.updated = FLUSH_CMD;
                uint32_t addr = manager->bus_line_addr.now - get_block_offset(manager->bus_line_addr.now); // Start answering from the start of the block
                manager->bus_line_addr.updated = addr;
                manager->bus_data.updated = manager->requestors[manager->interuptor_id]->myCore->cache_now.dsram[memory_address_to_cache_address(addr)];
                manager->bus_origid.updated = manager->interuptor_id;
                break;
            }
            break;
        }
        case BUS_BEFORE_FLUSH:
        {
            // Transition to BUS_FLUSH if 15 cycles have passed. Main memory shall answer
            if (*(manager->p_cycle) - manager->last_transaction_on_bus_cycle == MAIN_MEMORY_ANSWERING_DELAY - 1)
            {
                manager->bus_status.updated = BUS_FLUSH;
                manager->bus_origid.updated = MAIN_MEMORY_BUS_ORIGIN;
                manager->bus_cmd.updated = FLUSH_CMD;
                uint32_t addr_requested = manager->requestors[manager->core_turn.now]->address;
                uint32_t first_addr_answered = addr_requested - get_block_offset(addr_requested);
                manager->bus_line_addr.updated = first_addr_answered;
                manager->bus_data.updated = manager->main_memory->memory[first_addr_answered];
                manager->bus_shared.updated = manager->bus_shared.now;
            }
            else
            {
                manager->bus_status.updated = BUS_BEFORE_FLUSH;
                manager->bus_cmd.updated = NO_CMD;
                manager->bus_line_addr.updated = 0;
                manager->bus_data.updated = 0;
                manager->bus_origid.updated = 0;
                manager->bus_shared.updated = manager->bus_shared.now;
            }
            break;
        }
        case BUS_FLUSH:  // Happends if a core wishes to dump something / if memory answers
        {
            bool is_last_flush = false;
            if (get_block_offset(manager->bus_line_addr.now) == DATA_CACHE_BLOCK_DEPTH - 1)
                is_last_flush = true;

            if (!is_last_flush) // Continue in the flush operation
            {
                manager->bus_status.updated = BUS_FLUSH;
                manager->bus_cmd.updated = FLUSH_CMD;
                manager->bus_line_addr.updated = manager->bus_line_addr.now + 1;
                manager->bus_origid.updated = manager->bus_origid.now;
                manager->bus_shared.updated = manager->bus_shared.now;
                if (manager->bus_origid.now == MAIN_MEMORY_BUS_ORIGIN) // If it is the main memory who is flushing
                    manager->bus_data.updated = manager->main_memory->memory[manager->bus_line_addr.now + 1];
                else
                    manager->bus_data.updated = manager->requestors[manager->bus_origid.now]->myCore->cache_now.dsram[memory_address_to_cache_address(manager->bus_line_addr.now + 1)];
            }
            else
            {
                manager->interuptor_id = -1;
                manager->interrupted_by_another_snooper = false;

                // Do we need to go to bus free state
                if (manager->core_turn.updated == -1)
                {
                    manager->bus_status.updated = BUS_FREE;
                    manager->bus_cmd.updated = NO_CMD;
                    manager->bus_line_addr.updated = 0;
                    manager->bus_data.updated = 0;
                    manager->bus_origid.updated = 0;
                    manager->bus_shared.updated = BLOCK_NOT_SHARED;
                }
                else // i.e this is the last flush and another core got to enlist to the transaction to next cycle
                {
                    // Do nothing. write_bus_lines_from_requestor function did the job.
                }
            }

            // Update the main memory / cache according to the flush
            if (manager->bus_origid.now == MAIN_MEMORY_BUS_ORIGIN) // If it is the main memory who is flushing
            {
                // So we need to update the originator core who asked for the memory
                if (manager->requestors[manager->core_turn.now]->operation == BUS_RDX_CMD &&
                    manager->bus_line_addr.now == manager->requestors[manager->core_turn.now]->address)
                {
                    // Do nothing. The core wishes to write to this address, and not to be updated by the bus.
                }
                else
                {
                    manager->requestors[manager->core_turn.now]->myCore->cache_updated.dsram[get_index(manager->bus_line_addr.now) * DATA_CACHE_BLOCK_DEPTH +
                                                                                             get_block_offset(manager->bus_line_addr.now)] = manager->bus_data.now;
                }
                manager->requestors[manager->core_turn.now]->myCore->cache_updated.tsram[get_index(manager->bus_line_addr.now)].tag = get_tag(manager->bus_line_addr.now);
                if (is_last_flush) // Only than update the status of the block
                {
                    if (manager->bus_shared.now == BLOCK_NOT_SHARED && manager->requestors[manager->core_turn.now]->operation == BUS_RDX_CMD)
                        manager->requestors[manager->core_turn.now]->myCore->cache_updated.tsram[get_index(manager->bus_line_addr.now)].state = MODIFIED;
                    else if (manager->bus_shared.now == BLOCK_NOT_SHARED && manager->requestors[manager->core_turn.now]->operation == BUS_RD_CMD)
                        manager->requestors[manager->core_turn.now]->myCore->cache_updated.tsram[get_index(manager->bus_line_addr.now)].state = EXCLUSIVE;
                    else if (manager->bus_shared.now == BLOCK_SHARED && manager->requestors[manager->core_turn.now]->operation == BUS_RD_CMD)
                        manager->requestors[manager->core_turn.now]->myCore->cache_updated.tsram[get_index(manager->bus_line_addr.now)].state = SHARED;
                    else if (manager->bus_shared.now == BLOCK_SHARED && manager->requestors[manager->core_turn.now]->operation == BUS_RDX_CMD)
                    {
                        // It doesnt make sense, that someone shall answer with a block shared to a busRdX command
                        fprintf(stderr, "It doesnt make sense, that someone shall answer with a block shared to a busRdX command.\n");
                        exit(EXIT_FAILURE);
                    }
                }
            }
            else // I.E: A core wishes to dump something
                manager->main_memory->memory[manager->bus_line_addr.now] = manager->bus_data.now;

            break;
        }
        case BUS_CACHE_INTERRUPTED:
        {
            bool is_last_flush = false;
            if (get_block_offset(manager->bus_line_addr.now) == DATA_CACHE_BLOCK_DEPTH - 1)
                is_last_flush = true;

            if (!is_last_flush) // Continue in the flush operation
            {
                manager->bus_status.updated = BUS_CACHE_INTERRUPTED;
                manager->bus_cmd.updated = FLUSH_CMD;
                manager->bus_line_addr.updated = manager->bus_line_addr.now + 1;
                manager->bus_origid.updated = manager->bus_origid.now;
                manager->bus_shared.updated = manager->bus_shared.now;
                manager->bus_data.updated = manager->requestors[manager->bus_origid.now]->myCore->cache_now.dsram[memory_address_to_cache_address(manager->bus_line_addr.now + 1)];
            }
            else
            {
                manager->interuptor_id = -1;
                manager->interrupted_by_another_snooper = false;

                // Do we need to go to bus free state
                if (manager->core_turn.updated == -1)
                {
                    manager->bus_status.updated = BUS_FREE;
                    manager->bus_cmd.updated = NO_CMD;
                    manager->bus_line_addr.updated = 0;
                    manager->bus_data.updated = 0;
                    manager->bus_origid.updated = 0;
                    manager->bus_shared.updated = BLOCK_NOT_SHARED;
                }
                else // i.e this is the last flush and another core got to enlist to the transaction to next cycle
                {
                    // Do nothing. write_bus_lines_from_requestor function did the job.
                }
            }

            // So we need to update the originator core who asked for the memory
            if (manager->requestors[manager->core_turn.now]->operation == BUS_RDX_CMD &&
                manager->bus_line_addr.now == manager->requestors[manager->core_turn.now]->address)
            {
                // Do nothing. The core wishes to write to this address, and not to be updated by the bus.
            }
            else
            {
                manager->requestors[manager->core_turn.now]->myCore->cache_updated.dsram[memory_address_to_cache_address(manager->bus_line_addr.now)] = manager->bus_data.now;
            }
            manager->requestors[manager->core_turn.now]->myCore->cache_updated.tsram[get_index(manager->bus_line_addr.now)].tag = get_tag(manager->bus_line_addr.now);
            
            if (is_last_flush) // Only than update the status of the block
            {
                if (manager->bus_shared.now == BLOCK_NOT_SHARED && manager->requestors[manager->core_turn.now]->operation == BUS_RDX_CMD)
                    manager->requestors[manager->core_turn.now]->myCore->cache_updated.tsram[get_index(manager->bus_line_addr.now)].state = MODIFIED;
                else if (manager->bus_shared.now == BLOCK_NOT_SHARED && manager->requestors[manager->core_turn.now]->operation == BUS_RD_CMD)
                    manager->requestors[manager->core_turn.now]->myCore->cache_updated.tsram[get_index(manager->bus_line_addr.now)].state = EXCLUSIVE;
                else if (manager->bus_shared.now == BLOCK_SHARED && manager->requestors[manager->core_turn.now]->operation == BUS_RD_CMD)
                    manager->requestors[manager->core_turn.now]->myCore->cache_updated.tsram[get_index(manager->bus_line_addr.now)].state = SHARED;
                else if (manager->bus_shared.now == BLOCK_SHARED && manager->requestors[manager->core_turn.now]->operation == BUS_RDX_CMD)
                {
                    // It doesnt make sense, that someone shall answer with a block shared to a busRdX command
                    fprintf(stderr, "It doesnt make sense, that someone shall answer with a block shared to a busRdX command.\n");
                    exit(EXIT_FAILURE);
                }
            }

            // The main memory always listens
            manager->main_memory->memory[manager->bus_line_addr.now] = manager->bus_data.now;

            break;
        }
        default:
        {
            fprintf(stderr, "Unnown bus state.\n");
            exit(EXIT_FAILURE);
            break;
        }
    }

    // Update last transaction on bus if neccessary
    if (manager->bus_cmd.updated != NO_CMD)
        manager->last_transaction_on_bus_cycle = *(manager->p_cycle) + 1;

}

// Function to request an action from the bus (sets the request operation and address)
void enlist_to_bus(BusRequestor* requestor, int addr, BusCmd BusActionType, BusManager* manager)
{
    // Set the operation type and address if the requestor is free
    requestor->operation = BusActionType;
    requestor->address = addr;

    // Mark the requestor as occupied
    manager->enlisted_requestors[requestor->myCore->id] = requestor;
}

bool is_open_for_start_of_new_enlisting(BusManager* manager)
{
    // If the bus is free / this is the end of a transaction we are free to start a new one
    if (manager->bus_status.now == BUS_FREE ||
        (manager->bus_cmd.now == FLUSH_CMD &&
         get_block_offset(manager->bus_line_addr.now) == DATA_CACHE_BLOCK_DEPTH - 1))
        return true;
    return false;
}

// function to call when bus needs to be interupted (state m and busrd/busrdx)
void interrupt_bus(BusManager* manager, int32_t interruptor_id)
{
    manager->interrupted_by_another_snooper = true;
    manager->interuptor_id = interruptor_id;
}

// Function to advance the bus to the next cycle
void advance_bus_to_next_cycle(BusManager* manager)
{
    UPDATE_FLIP_FLOP(manager->bus_status, false);
    UPDATE_FLIP_FLOP(manager->bus_shared, false);
    UPDATE_FLIP_FLOP(manager->bus_cmd, false);
    UPDATE_FLIP_FLOP(manager->bus_origid, false);
    UPDATE_FLIP_FLOP(manager->bus_line_addr, false);
    UPDATE_FLIP_FLOP(manager->bus_data, false);

    // The core turn stays the same.
    // it is changed only in finish_bus_enlisting.
    // which occurs only when there is a possibility to enter the bus in the next cycle
    UPDATE_FLIP_FLOP(manager->core_turn, true); 
}
