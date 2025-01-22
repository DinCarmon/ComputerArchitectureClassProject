#ifndef CORE_H
#define CORE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cache_block.h"        // Include the Cache definition
#include "flip_flop.h"
#include "bus_snooper.h"
#include "bus_requestor.h"



// Define the Core struct
typedef struct {
    int id;                          //core id
    FlipFlop_uint32_t pc_register;    // flip flop to hold the program counter
    FlipFlop_uint32_t registers[32];            //flip flop to hold the state of the registers
    uint32_t InstructionMemory[1024];  // Instruction memory array
    Cache cache_now;                // Pointer to the current cache
    Cache cache_updated;            // Pointer to the updated cache
    //WriteBackStage writeback_stage;
    //ExecuteStage execute_stage;
    //DecodeStage decode_stage;
    //FetchStage fetch_stage;
    //MemoryStage memory_stage;
    BusRequestor requestor;
    BusSnooper snooper;
} Core;

// Function to initialize a Core instance
Core core_create(int id);

// Function to advance the core to the next cycle
void advance_core(Core* core, bool keep_value);

#endif