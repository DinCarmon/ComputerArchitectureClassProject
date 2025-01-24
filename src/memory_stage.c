#include "memory_stage.h"
#include "bus_manager.h"
#include "core.h"

bool handleCacheHit(MemoryStage* self)
{
    if (self->state.inputState.instruction.opcode == Lw)
    {
        self->state.outputState.memoryRetrieved = read_cache(self->state.inputState.aluOperationOutput,
                                                              &(self->state.myCore->cache_now));
        self->num_of_cycles_on_same_command = 0;
        return false;
    }

    // From here on we can assume the opcode is Sw

    // If the block is not shared. i.e it is exclsive / modified, there is no need to do a transaction on the bus.
    if (get_state(self->state.inputState.aluOperationOutput,
                  &(self->state.myCore->cache_now))
            != SHARED)
    {
        write_cache(self->state.inputState.aluOperationOutput,
                    &(self->state.myCore->cache_updated),
                    self->state.inputState.rdValue);
        self->num_of_cycles_on_same_command = 0;
        return false;
    }

    // From here we can assume the block is shared.
    // Therefore, we need to send a BusRdX command before we can move on

	// checks if the wished busRdx transaction is on the bus right now.
    if (self->state.myCore->bus_manager->bus_cmd.now == BUS_RDX_CMD &&
        self->state.myCore->bus_manager->bus_origid.now == self->state.myCore->id &&
        self->state.myCore->bus_manager->bus_line_addr.now == self->state.inputState.aluOperationOutput)
    {
        write_cache(self->state.inputState.aluOperationOutput,
                    &(self->state.myCore->cache_updated),
                    self->state.inputState.rdValue);
        self->num_of_cycles_on_same_command = 0;
        return false;
    }

    // If we got here, it means the transaction was not sent yet.
    // Therefore, we try to enlist again for a bus transaciton.
    if (is_open_for_start_of_new_enlisting(self->state.myCore->bus_manager))
    {
        enlist_to_bus(&(self->state.myCore->requestor),
                      self->state.inputState.aluOperationOutput,
                      BUS_RDX_CMD,
                      self->state.myCore->bus_manager);
        self->state.myCore->requestor.data = self->state.inputState.rdValue;
    }
        

    // We need to stall. only in next round we shall now if we were granted access to the bus.
    self->num_of_cycles_on_same_command++;
    return true;
}

bool handleCacheMiss(MemoryStage* self)
{
    // If now it is the end of our transaction. we can look at the bus right now and move on
    // at this cycle
    if (get_block_offset(self->state.myCore->bus_manager->bus_line_addr.now) == DATA_CACHE_BLOCK_DEPTH - 1 &&
        get_index(self->state.myCore->bus_manager->bus_line_addr.now) == get_index(self->state.inputState.aluOperationOutput) &&
        self->state.myCore->bus_manager->core_turn.now == self->state.myCore->id &&
        self->state.myCore->bus_manager->bus_cmd.now == FLUSH_CMD)
    {
        if (self->state.inputState.instruction.opcode == Sw)
        {
            write_cache(self->state.inputState.aluOperationOutput,
                        &(self->state.myCore->cache_updated),
                        self->state.inputState.rdValue);
        }
        else // a load command
        {
            if (get_block_offset(self->state.inputState.aluOperationOutput) == DATA_CACHE_BLOCK_DEPTH - 1) // If it is now on the bus
                self->state.outputState.memoryRetrieved = self->state.myCore->bus_manager->bus_data.now;
            else // If the required address was sent in one of the last 3 cycles, it is already in the cache
                self->state.outputState.memoryRetrieved = self->state.myCore->cache_now.dsram[get_index(self->state.inputState.aluOperationOutput) * DATA_CACHE_BLOCK_DEPTH +
                                                                                              get_block_offset(self->state.inputState.aluOperationOutput)];
        }

        self->num_of_cycles_on_same_command = 0;
        return false;
    }

    // If there is a transaction in progress on the bus in the next cycle,
    // Than it means that either our transaction not yet finished.
    // or that we cant send our request now.
    if (is_open_for_start_of_new_enlisting(self->state.myCore->bus_manager) == false)
    {
        self->num_of_cycles_on_same_command++;
		return true;
    }

    // If we got here it means we can enlist to the bus, but there is an edge case in which
    // the current transaction is already on the bus.
    // We check if this is the case:
    if (self->state.myCore->bus_manager->core_turn.now == self->state.myCore->id &&
        self->state.myCore->requestor.address == self->state.inputState.aluOperationOutput)
    {
        self->num_of_cycles_on_same_command++;
		return true;
	}

    // If we got here, we can assume the request is not on the bus. I.E we need to enlist again for the bus.
    // First, we need to find out what request we wish to send to the bus.

    // Do we need to flush first? Check if the block which is in the same index in cache needs is modified.
    if (get_state(get_first_address_in_block(&(self->state.myCore->cache_now),
                                             self->state.inputState.aluOperationOutput),
                  &(self->state.myCore->cache_now)) == MODIFIED)
    {
        if (is_open_for_start_of_new_enlisting(self->state.myCore->bus_manager))
            enlist_to_bus(&(self->state.myCore->requestor), 
                          get_first_address_in_block(&(self->state.myCore->cache_now),
                                                     self->state.inputState.aluOperationOutput),
                          FLUSH_CMD,
                          self->state.myCore->bus_manager);
        self->num_of_cycles_on_same_command++;
		return true;
    }

    // If we got here, it means we can overwrite the block currently in cache / the index in cache is free.
    // Check which transaction we wish to send.
    
    if (self->state.inputState.instruction.opcode == Lw)
    {
        if (is_open_for_start_of_new_enlisting(self->state.myCore->bus_manager))
            enlist_to_bus(&(self->state.myCore->requestor),
                          self->state.inputState.aluOperationOutput,
                          BUS_RD_CMD,
                          self->state.myCore->bus_manager);
        self->num_of_cycles_on_same_command++;
        return true;
    }
    else if (self->state.inputState.instruction.opcode == Sw)
    {
        if (is_open_for_start_of_new_enlisting(self->state.myCore->bus_manager))
        {
            enlist_to_bus(&(self->state.myCore->requestor),
                          self->state.inputState.aluOperationOutput,
                          BUS_RDX_CMD,
                          self->state.myCore->bus_manager);
            self->state.myCore->requestor.data = self->state.inputState.rdValue;
        }
        self->num_of_cycles_on_same_command++;
        return true;
    }
    
    // Runtime should never reach here.
    printf("This is weird. Code should not get here");
    exit(-1);
    return false;
}

bool do_memory_operation(MemoryStage* self)
{
    // First copy the output state from the input state.
    // Later update the output state with operation needed to be
    // done at this round
    self->state.outputState = self->state.inputState;

    if (self->state.inputState.instruction.opcode != Lw &&
        self->state.inputState.instruction.opcode != Sw)
    {
        self->num_of_cycles_on_same_command = 0;
        self->state.inputState.is_ready = false;
        self->state.outputState.is_ready = true;
        return false;
    }

    if (in_cache(self->state.inputState.aluOperationOutput,
                 &(self->state.myCore->cache_now)))
    {
        // If this is the first time we try to run the command
        if (self->num_of_cycles_on_same_command == 0 && self->state.inputState.instruction.opcode == Lw)
            self->state.myCore->num_read_hits++;
        if (self->num_of_cycles_on_same_command == 0 && self->state.inputState.instruction.opcode == Sw)
            self->state.myCore->num_write_hits++;

        bool ret = handleCacheHit(self);
        if (ret == false)
        {
            self->state.inputState.is_ready = false;
            self->state.outputState.is_ready = true;
        }
        else
        {
            self->state.inputState.is_ready = true;
            self->state.outputState.is_ready = false;
        }

        return ret;
    }
    else
    {
        // If this is the first time we try to run the command
        if (self->num_of_cycles_on_same_command == 0 && self->state.inputState.instruction.opcode == Lw)
            self->state.myCore->num_read_miss++;
        if (self->num_of_cycles_on_same_command == 0 && self->state.inputState.instruction.opcode == Sw)
            self->state.myCore->num_write_miss++;

        bool ret = handleCacheMiss(self);
        if (ret == false)
        {
            self->state.inputState.is_ready = false;
            self->state.outputState.is_ready = true;
        }
        else
        {
            self->state.inputState.is_ready = true;
            self->state.outputState.is_ready = false;
        }

        return ret;
    }

    // Code run should not get here.
    printf("Code should not get here!");
    exit(-1);
    return false;
}

void configure_memory_stage(MemoryStage* stage, struct core* myCore)
{
    stage->num_of_cycles_on_same_command = 0;
    configure_stage_data(&(stage->state), myCore);
}