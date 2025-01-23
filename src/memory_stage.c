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
		update_state(self->state.inputState.rdValue,
			         &(self->state.myCore->cache_updated),
			         MODIFIED);
        self->num_of_cycles_on_same_command = 0;
        return false;
    }

    // From here we can assume the block is shared.
    // Therefore, we need to send a BusRdX command before we can move on

	// checks if the wished busRdx transaction is on the bus right now.
    if (self->state.myCore->requestor.is_request_on_bus.now &&
        self->num_of_cycles_on_same_command > 0)
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
        enlist_to_bus(&(self->state.myCore->requestor),
                      self->state.inputState.aluOperationOutput,
                      BUS_RDX_CMD,
                      self->state.myCore->bus_manager);

    // We need to stall. only in next round we shall now if we were granted access to the bus.
    self->num_of_cycles_on_same_command++;
    return true;
}

bool handleCacheMiss(MemoryStage* self)
{
    // If we already sent the request, there is nothing to do, but wait for it to end.
	if (self->state.myCore->requestor.is_request_on_bus.now)
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
            enlist_to_bus(&(self->state.myCore->requestor),
                          self->state.inputState.aluOperationOutput,
                          BUS_RDX_CMD,
                          self->state.myCore->bus_manager);
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

        return handleCacheHit(self);
    }
    else
    {
        // If this is the first time we try to run the command
        if (self->num_of_cycles_on_same_command == 0 && self->state.inputState.instruction.opcode == Lw)
            self->state.myCore->num_read_miss++;
        if (self->num_of_cycles_on_same_command == 0 && self->state.inputState.instruction.opcode == Sw)
            self->state.myCore->num_write_miss++;

        return handleCacheMiss(self);
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