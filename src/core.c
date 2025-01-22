#include "core.h"


// Function to initialize a Core instance on the heap
Core core_create(int id) {
    // Allocate memory for the Core structure
    Core newCore;

    // Initialize fields
    newCore.id = id;
    newCore.pc_register.now = 0;
    newCore.pc_register.updated = 0;

    for (int i = 0; i < NUM_REGISTERS_PER_CORE; i++)
    {
        newCore.registers[i].now = 0;
        newCore.registers[i].updated = 0;
    }

    for (int i = 0; i < INSTRUCTION_MEMORY_DEPTH; i++)
    {
        newCore.InstructionMemory[i] = 0;
    }

    // Initialize the caches
    newCore.cache_now = cache_create();
    newCore.cache_updated = cache_create();
    //newCore.decode_stage = createDecodeStage();
    //newCore.decode_stage.state.myCore = newCore;
    //newCore.memory_stage = createMemoryStage();
    //newCore.memory_stage.state.myCore = newCore;
    //newCore.execute_stage = createExecuteStage();
    //newCore.execute_stage.state.myCore = newCore;
    //newCore.fetch_stage = createFetchStage();
    //newCore.fetch_stage.state.myCore = newCore;
    //newCore.writeback_stage = createWriteBackStage();
    //newCore.writeback_stage.state.myCore = newCore;

    newCore.requestor = bus_requestor_create(newCore.id);
    newCore.snooper = bus_snooper_create(newCore.id); 

    return newCore;
}

// Function to advance the core to the next cycle
void advance_core(Core* core, bool keep_value) {
    UPDATE_FLIP_FLOP(core->pc_register, keep_value);
    for (int i = 0; i < 32; i++) {
        UPDATE_FLIP_FLOP(core->registers[i], keep_value);
    }
    UPDATE_FLIP_FLOP(core->snooper.busSnooperActive, true);
    UPDATE_FLIP_FLOP(core->requestor.IsRequestOnBus, true);
    UPDATE_FLIP_FLOP(core->requestor.LastCycle, true);


    for (int i = 0; i < DSRAM_SIZE; i++)
    {
        core->cache_now.dsram[i] = core->cache_updated.dsram[i];
    }
    for (int i = 0; i < TSRAM_SIZE; i++)
    {
        core->cache_now.tsram[i] = core->cache_updated.tsram[i];
    }
}

