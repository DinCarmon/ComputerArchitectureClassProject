#ifndef CORE_H
#define CORE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fetch_stage.h"
#include "decode_stage.h"
#include "execute_stage.h"
#include "memory_stage.h"
#include "writeback_stage.h"
#include "cache_block.h"        // Include the Cache definition
#include "flip_flop.h"
#include "bus_snooper.h"
#include "bus_requestor.h"

struct busManager;

// Define the Core struct
typedef struct core {
    int id;                          //core id
    FlipFlop_uint32_t pc_register;    // flip flop to hold the program counter
    FlipFlop_uint32_t registers[32];            //flip flop to hold the state of the registers
    uint32_t InstructionMemory[1024];  // Instruction memory array
    Cache cache_now;                // Pointer to the current cache
    Cache cache_updated;            // Pointer to the updated cache

    struct fetchStage fetch_stage;
    struct decodeStage decode_stage;
    struct executeStage execute_stage;
    struct memoryStage memory_stage;
    struct writeBackStage writeback_stage;

    BusRequestor requestor;
    BusSnooper snooper;

    struct busManager* bus_manager;

    uint64_t* p_cycle;
    uint64_t halt_cycle;                        // The cycle in which a halt arrived to the writeback stage the operation of the core was finished.

    uint64_t num_of_instructions_executed;      // Not including the halt final command.

    uint64_t num_of_decode_stage_stalls;
    uint64_t num_of_memory_stage_stalls;

    uint64_t num_read_hits;
    uint64_t num_write_hits;
    uint64_t num_read_miss;
    uint64_t num_write_miss;
} Core;

// Function to initialize a Core instance
void configure_core(Core* core, int id, struct busManager* manager);

// Function to advance the core to the next cycle
void advance_core(Core* core,
                  uint64_t last_succesful_writeback_execution,
                  uint64_t last_succesful_memory_execution,
                  uint64_t last_succesful_execute_execution,
                  uint64_t last_succesful_decode_execution,
                  uint64_t last_succesful_fetch_execution,
                  uint64_t last_insuccesful_memory_execution,
                  uint64_t last_insuccesful_decode_execution);

#endif