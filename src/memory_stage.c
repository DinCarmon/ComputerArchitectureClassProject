#include "memory_stage.h"
#include "defines.h"

bool handleCacheHit(MemoryStage* self, BusManager* manager)
{
    if (self->state.inputState.instruction.opcode == Lw)
    {
        self->state.outputState.memoryRetrieved = read_cache(self->state.inputState.aluOperationOutput,
                                                            self->state.myCore->cache_now);
        return false;
    }

    // From here on we can assume the opcode is Sw

    if (get_state(self->state.inputState.aluOperationOutput,
                 self->state.myCore->cache_now)
            != Shared)
    {
        write_cache(self->state.inputState.aluOperationOutput,
                    self->state.myCore->cache_updated,
                    self->state.inputState.rdValue);
		update_state(self->state.inputState.rdValue,
			self->state.myCore->cache_updated,
			Modified);
        return false;
    }

    // From here we can assume the block is shared.
    // Therefore, we need to send a BusRdX command before we can move on

    // If the current transaction is the transaction we wish to send, it means that in previous
    // cycles we asked to send it and we finally got to send the transaction on the bus.
    // Now, we can update the cache - we do not need to wait for the transaction to end.
    //if (isCurrentTransactionOnBusOriginId(self->state.myCore->id)   &&
    //    isCurrentTransactionOnBusCmd(BusRdX)                        &&
    //    isCurrentTransactionOnBusAddr(self->state.inputState.aluOperationOutput))
    //{
    //    writeCache(self->state.inputState.aluOperationOutput,
    //                self->state.myCore->cache_updated,
    //                self->state.inputState.rdValue);
    //    return false;
    //}

	// checks if my transaction is on the bus and finished before updating the cache
    if (self->state.myCore->requestor->IsRequestOnBus.now &&
        self->state.myCore->requestor->LastCycle.now) {

        write_cache(self->state.inputState.aluOperationOutput,
            self->state.myCore->cache_updated,
            self->state.inputState.rdValue);
    }

    //if (busRequestorAlreadyOccupied(self->state.myCore->requestor))
    //{
    //    return true;
    //}

	if (self->state.myCore->requestor->IsRequestOnBus.now)  // i think this is what you meant
    {
        return true;
    }


    // From here we can assume our bus requestor is free.
    // We can ask it to send a transaction of BusRdx

    // When the BusRdX transaction shall finish, the bus requestor shall
    // change the cache status bits to Modified.
    // The cache update itself shall be done by this function in previous lines in future cycle.
    //requestActionFromBusRequestor(self->state.inputState.aluOperationOutput,
    //   
    //                           BusRdX);
	self->state.myCore->requestor->operation = BusRdX;
	self->state.myCore->requestor->address = self->state.inputState.aluOperationOutput;
	Enlist(self->state.myCore->requestor, self->state.inputState.aluOperationOutput, BusRdX, manager);

    return true;
}

bool handleCacheMiss(MemoryStage* self, BusManager* manager)
{
    //if (busRequestorAlreadyOccupied(self->state.myCore->requestor))
    //    return true;

	if (self->state.myCore->requestor->IsRequestOnBus.now)
	{
		return true;
	}

    // Do we need to flush first?
    if (get_state(self->state.inputState.aluOperationOutput, self->state.myCore->cache_now)
        == Modified)
        //{
        //    requestActionFromBusRequestor(getFirstAddressInBlock(get_index(self->state.inputState.aluOperationOutput),
        //                                                         self->state.myCore->cache_updated),
        //                                  Flush);
        //    return true;
        //}

    {
		self->state.myCore->requestor->operation = Flush;
		self->state.myCore->requestor->address = self->state.inputState.aluOperationOutput;
		Enlist(self->state.myCore->requestor, self->state.myCore->requestor->address, Flush, manager);
		return true;
    }
    
    if (self->state.inputState.instruction.opcode == Lw)
    {
        //requestActionFromBusRequestor(self->state.inputState.aluOperationOutput,
        //                              BusRd);

		self->state.myCore->requestor->operation = BusRd;
		self->state.myCore->requestor->address = self->state.inputState.aluOperationOutput;
		Enlist(self->state.myCore->requestor, self->state.inputState.aluOperationOutput, BusRd, manager);
        return true;
    }
    else if (self->state.inputState.instruction.opcode == Sw)
    {
        //requestActionFromBusRequestor(self->state.inputState.aluOperationOutput,
        //                              BusRdX);
		self->state.myCore->requestor->operation = BusRdX;
		self->state.myCore->requestor->address = self->state.inputState.aluOperationOutput;
		Enlist(self->state.myCore->requestor, self->state.inputState.aluOperationOutput, BusRdX, manager);
        return true;
    }
    
    // Runtime should never reach here.
    return false;
}

bool doMemoryOperation(MemoryStage* self, BusManager* manager)
{
    // First copy the output state from the input state.
    // Later update the output state with operation needed to be
    // done at this round
    self->state.outputState = self->state.inputState;

    if (self->state.inputState.instruction.opcode != Lw &&
        self->state.inputState.instruction.opcode != Sw)
        return false;

    if (in_cache(self->state.inputState.aluOperationOutput,
               self->state.myCore->cache_now))
    {
        return handleCacheHit(self, manager);
    }
    else
    {
        return handleCacheMiss(self, manager);
    }
    return false;
}

MemoryStage createMemoryStage()
{
    MemoryStage stage;
    stage.doOperation = doMemoryOperation;

    return stage;
}