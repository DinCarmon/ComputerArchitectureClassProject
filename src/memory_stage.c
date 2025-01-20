#include "memory_stage.h"

bool handleCacheHit(MemoryStage* self)
{
    if (self->state.inputState.instruction.opcode == Lw)
    {
        self->state.outputState.memoryRetrieved = readCache(self->state.inputState.aluOperationOutput,
                                                            self->state.myCore->cache);
        return false;
    }

    // From here on we can assume the opcode is Sw

    if (getState(self->state.inputState.aluOperationOutput,
                 self->state.myCore->cache)
            != Shared)
    {
        writeCache(self->state.inputState.aluOperationOutput,
                    self->state.myCore->cache,
                    self->state.inputState.rdValue);
        return false;
    }

    // From here we can assume the block is shared.
    // Therefore, we need to send a BusRdX command before we can move on

    // If the current transaction is the transaction we wish to send, it means that in previous
    // cycles we asked to send it and we finally got to send the transaction on the bus.
    // Now, we can update the cache - we do not need to wait for the transaction to end.
    if (isCurrentTransactionOnBusOriginId(self->state.myCore->id)   &&
        isCurrentTransactionOnBusCmd(BusRdX)                        &&
        isCurrentTransactionOnBusAddr(self->state.inputState.aluOperationOutput))
    {
        writeCache(self->state.inputState.aluOperationOutput,
                    self->state.myCore->cache,
                    self->state.inputState.rdValue);
        return false;
    }

    if (busRequestorAlreadyOccupied(self->state.myCore.busRequestor))
    {
        return true;
    }

    // From here we can assume our bus requestor is free.
    // We can ask it to send a transaction of BusRdx

    // When the BusRdX transaction shall finish, the bus requestor shall
    // change the cache status bits to Modified.
    // The cache update itself shall be done by this function in previous lines in future cycle.
    requestActionFromBusRequestor(self->state.inputState.aluOperationOutput,
                                  BusRdX);

    return true;
}

bool handleCacheMiss(MemoryStage* self)
{
    if (busRequestorAlreadyOccupied(self->state.myCore.busRequestor))
        return true;

    // Do we need to flush first?
    if (getState(getFirstAddressInBlock(getIndex(self->state.inputState.aluOperationOutput),
                                        self->state.myCore->cache),
                 self->state.myCore->cache)
            == Modified)
        {
            requestActionFromBusRequestor(getFirstAddressInBlock(getIndex(self->state.inputState.aluOperationOutput),
                                                                 self->state.myCore->cache),
                                          Flush);
            return true;
        }
    
    if (self->state.inputState.instruction.opcode == Lw)
    {
        requestActionFromBusRequestor(self->state.inputState.aluOperationOutput,
                                      BusRd);
        return true;
    }
    else if (self->state.inputState.instruction.opcode == Sw)
    {
        requestActionFromBusRequestor(self->state.inputState.aluOperationOutput,
                                      BusRdX);
        return true;
    }
    
    // Runtime should never reach here.
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

    if (inCache(self->state.inputState.aluOperationOutput,
               self->state.myCore->cache))
    {
        return handleCacheHit(self);
    }
    else
    {
        return handleCacheMiss(self);
    }
    return false;
}

MemoryStage createMemoryStage()
{
    MemoryStage stage;
    stage.doOperation = doMemoryOperation;

    return stage;
}