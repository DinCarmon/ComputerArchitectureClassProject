#include "core.h"
#include "bus_manager.h"

// Function to initialize a Core instance on the heap
void configure_core(Core* core, int id, BusManager* bus_manager)
{
    // Initialize fields
    core->id = id;
    core->pc_register.now = 0;
    core->pc_register.updated = 0;
    core->num_of_instructions_executed = 0;
    core->num_of_decode_stage_stalls = 0;
    core->num_of_memory_stage_stalls = 0;
    core->num_read_hits = 0;
    core->num_read_miss = 0;
    core->num_write_hits = 0;
    core->num_write_miss = 0;

    for (int i = 0; i < NUM_REGISTERS_PER_CORE; i++)
    {
        core->registers[i].now = 0;
        core->registers[i].updated = 0;
    }

    for (int i = 0; i < INSTRUCTION_MEMORY_DEPTH; i++)
    {
        core->InstructionMemory[i] = 0;
    }

    // Initialize the caches
    reset_cache(&(core->cache_now));
    reset_cache(&(core->cache_updated));

    // Initialize each stage
    configure_fetch_stage(&(core->fetch_stage), core);
    configure_decode_stage(&(core->decode_stage), core);
    configure_execute_stage(&(core->execute_stage), core);
    configure_memory_stage(&(core->memory_stage), core);
    configure_writeback_stage(&(core->writeback_stage), core);

    // Initialize bus snopper and bus requestor
    configure_bus_requestor(&(core->requestor), core);
    configure_bus_snooper(&(core->snooper), core);

    // Connect bus manager
    core->bus_manager = bus_manager;
}

// Function to advance the core to the next cycle
void advance_core(Core* core,
                  uint64_t last_succesful_writeback_execution,
                  uint64_t last_succesful_memory_execution,
                  uint64_t last_succesful_execute_execution,
                  uint64_t last_succesful_decode_execution,
                  uint64_t last_succesful_fetch_execution,
                  uint64_t last_insuccesful_memory_execution,
                  uint64_t last_insuccesful_decode_execution)
{
    // Update flip flops between stages
    if ((last_succesful_fetch_execution == *(core->p_cycle) &&
         last_insuccesful_decode_execution != *(core->p_cycle)) ||
        (last_succesful_decode_execution == *(core->p_cycle)))
        core->decode_stage.state.inputState = core->fetch_stage.state.outputState;
    if ((last_succesful_decode_execution == *(core->p_cycle)) ||
        (last_insuccesful_decode_execution == *(core->p_cycle) &&
         last_succesful_execute_execution == *(core->p_cycle) ))
        core->execute_stage.state.inputState = core->decode_stage.state.outputState;
    if ((last_succesful_execute_execution == *(core->p_cycle) &&
         last_insuccesful_memory_execution != *(core->p_cycle)) ||
        (last_succesful_memory_execution == *(core->p_cycle)))
        core->memory_stage.state.inputState = core->execute_stage.state.outputState;
    if ((last_succesful_memory_execution == *(core->p_cycle)) ||
        (last_insuccesful_memory_execution == *(core->p_cycle) &&
         last_succesful_writeback_execution == *(core->p_cycle) ))
        core->writeback_stage.state.inputState = core->memory_stage.state.outputState;

    // Update the num of stalls
    if(last_insuccesful_decode_execution == *(core->p_cycle))
    {
        core->num_of_decode_stage_stalls++;
    }
    if(last_insuccesful_memory_execution == *(core->p_cycle))
    {
        core->num_of_memory_stage_stalls++;
    }

    // Update registers
    UPDATE_FLIP_FLOP(core->pc_register, false);
    for (int i = 0; i < 32; i++) {
        UPDATE_FLIP_FLOP(core->registers[i], true);
    }

    // Update snooper
    UPDATE_FLIP_FLOP(core->snooper.busSnooperActive, true);

    // Update requestor
    UPDATE_FLIP_FLOP(core->requestor.IsRequestOnBus, true);
    UPDATE_FLIP_FLOP(core->requestor.LastCycle, true);

    // Update cache
    for (int i = 0; i < DSRAM_SIZE; i++)
    {
        core->cache_now.dsram[i] = core->cache_updated.dsram[i];
    }
    for (int i = 0; i < TSRAM_SIZE; i++)
    {
        core->cache_now.tsram[i] = core->cache_updated.tsram[i];
    }
}

