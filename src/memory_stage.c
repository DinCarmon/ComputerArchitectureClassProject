#include "memory_stage.h"
#include "bus_manager.h"
#include "core.h"

bool handleCacheHit(MemoryStage* self)
{
    if (self->state.inputState.instruction.opcode == Lw)
    {
        self->state.outputState.memoryRetrieved = read_cache(self->state.inputState.aluOperationOutput,
                                                              &(self->state.myCore->cache_now));
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
        return false;
    }

    // From here we can assume the block is shared.
    // Therefore, we need to send a BusRdX command before we can move on

	// checks if the wished busRdx transaction is on the bus right now.
    if (self->state.myCore->requestor.IsRequestOnBus.now) {
        write_cache(self->state.inputState.aluOperationOutput,
                    &(self->state.myCore->cache_updated),
                    self->state.inputState.rdValue);
        return false;
    }

    // If we got here, it means the transaction was not sent yet.
    // Therefore, we try to enlist again for a bus transaciton.
	Enlist(&(self->state.myCore->requestor),
           self->state.inputState.aluOperationOutput,
           BusRdX,
           self->state.myCore->bus_manager);

    // We need to stall. only in next round we shall now if we were granted access to the bus.
    return true;
}

bool handleCacheMiss(MemoryStage* self)
{
    // If we already sent the request, there is nothing to do, but wait for it to end.
	if (self->state.myCore->requestor.IsRequestOnBus.now)
	{
		return true;
	}

    // If we got here, we can assume the request is not on the bus. I.E we need to enlist again for the bus.
    // First, we need to find out what request we wish to send to the bus.

    // Do we need to flush first? Check if the block which is in the same index in cache needs is modified.
    if (get_state(get_first_address_in_block(&(self->state.myCore->cache_now),
                                             self->state.inputState.aluOperationOutput),
                  &(self->state.myCore->cache_now)) == MODIFIED)
    {
		Enlist(&(self->state.myCore->requestor), 
               get_first_address_in_block(&(self->state.myCore->cache_now),
                                          self->state.inputState.aluOperationOutput),
               Flush,
               self->state.myCore->bus_manager);
		return true;
    }

    // If we got here, it means we can overwrite the block currently in cache / the index in cache is free.
    // Check which transaction we wish to send.
    
    if (self->state.inputState.instruction.opcode == Lw)
    {
		Enlist(&(self->state.myCore->requestor),
               self->state.inputState.aluOperationOutput,
               BusRd,
               self->state.myCore->bus_manager);
        return true;
    }
    else if (self->state.inputState.instruction.opcode == Sw)
    {
		Enlist(&(self->state.myCore->requestor),
               self->state.inputState.aluOperationOutput,
               BusRdX,
               self->state.myCore->bus_manager);
        return true;
    }
    
    // Runtime should never reach here.
    printf("This is weird. Code should not get here");
    exit(-1);
    return false;
}

bool doMemoryOperation(MemoryStage* self)
{
    // First copy the output state from the input state.
    // Later update the output state with operation needed to be
    // done at this round
    self->state.outputState = self->state.inputState;

    if (self->state.inputState.instruction.opcode != Lw &&
        self->state.inputState.instruction.opcode != Sw)
        return false;

    if (in_cache(self->state.inputState.aluOperationOutput,
                 &(self->state.myCore->cache_now)))
    {
        return handleCacheHit(self);
    }
    else
    {
        return handleCacheMiss(self);
    }

    // Code run should not get here.
    printf("Code should not get here!");
    exit(-1);
    return false;
}

MemoryStage createMemoryStage()
{
    MemoryStage stage;
    stage.doOperation = doMemoryOperation;

    return stage;
}