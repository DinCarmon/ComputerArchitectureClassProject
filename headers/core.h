#pragma once
#include <stdio.h>
#include <stdlib.h> // For malloc and free
#include <string.h> // For memset
#include "cache.h"  // Include the Cache definition
#include "flip_flop.h"
#include "defines.h"
#include "writeback_stage.h"
#include "fetch_stage.h"
#include "decode_stage.h"
#include "execute_stage.h"
#include "memory_stage.h"
#include "bus_snooper.h"
#include "bus_request.h"


// Define the Core struct
typedef struct {
    int id;                          //core id
    FlipFlop_uint32_t pc_register;    // flip flop to hold the program counter
    FlipFlop_uint32_t registers[32];            //flip flop to hold the state of the registers
    uint32_t InstructionMemory[1024];  // Instruction memory array
    Cache* cache_now;                // Pointer to the current cache
    Cache* cache_updated;            // Pointer to the updated cache
    WriteBackStage writeback_stage;
    ExecuteStage execute_stage;
    DecodeStage decode_stage;
    FetchStage fetch_stage;
    MemoryStage memory_stage;
    BusRequestor* requestor;
    BusSnooper* snooper;
} Core;

// Function to initialize a Core instance
Core* core_create(int id);

// function to create an arry of core pointers
Core * *create_cores_array(int numCores);

// Function to free allocated resources in a Core instance
void core_destroy(Core* core);

// Destroys an entire array of Core pointers and the array itself
void destroy_cores_array(Core** cores, int numCores);

// Function to advance the core to the next cycle
void advance_core(Core* core, bool keep_value);

/**
 * Creates and returns a new array of BusRequestor* by gathering
 * them from each Core in `cores`.
 *
 * @param cores     An array of pointers to Core
 * @param numCores  Number of elements in the `cores` array
 *
 * @return          A newly allocated array of BusRequestor*.
 *                  The caller is responsible for freeing this array.
 *                  (The BusRequestor objects themselves are still owned by each Core.)
 */
BusRequestor** create_busrequestors_array(Core** cores, int numCores);

/**
 * Frees the dynamically allocated array of BusRequestor*.
 * This does NOT free the BusRequestors themselves (they
 * belong to their respective Core).
 *
 * @param requestors  The array of BusRequestor* to free.
 */
void destroy_busrequestors_array(BusRequestor** requestors);
