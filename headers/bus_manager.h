#ifndef BUS_MANAGER_H
#define BUS_MANAGER_H

#include "bus_requestor.h"
#include "main_memory.h"
#include "constants.h"
#include "cache_block.h"
#include "flip_flop.h"

struct core;

typedef struct busManager {
    uint32_t LastTransactionCycle;                      // The last cycle a transaction occurred. // 
    FlipFlop_int32_t BusStatus;                         // status of the bus
    FlipFlop_bool Interupted;                           // is the bus on interrupted mode. If another cache wishes to answer.
    int32_t interuptor_id;                              // id of the interupting core 
    FlipFlop_bool bus_shared;                           // True if the bus is in shared mode
    FlipFlop_int32_t bus_origid;                        // Originating ID for the current transaction
    FlipFlop_int32_t bus_cmd;                           // Command being executed on the bus
    FlipFlop_uint32_t bus_addr;                         // Address involved in the bus transaction
    FlipFlop_uint32_t bus_data;                         // Data being transferred on the bus.
    FlipFlop_uint32_t bus_line_addr;                    // the address that will go on the bus lines
    int32_t core_turn;                                  // Core that has the current turn
    uint32_t numOfCyclesInSameStatus;                   // cycels in one transaction
    BusRequestor* requestors[NUM_OF_CORES];             // Fixed array of BusRequestors
    BusRequestor* enlisted_requestors[NUM_OF_CORES];    // List of enlisted BusRequestors
    struct core* cores[NUM_OF_CORES];                   // fixed array of caches
    MainMemory* main_memory;                            // pointer to the main memory

    uint64_t* p_cycle;
} BusManager;

// Function to create and initialize a BusManager
void configure_bus_manager(BusManager* manager, struct core** cores, MainMemory* main_memory);

// Function to assign the core with the lowest priority to core_turn
void do_bus_operation(BusManager* manager);

// Function to write to the bus data line
void writeBusData(BusManager* manager, bool read, Cache* cache);

// Function to advance the bus to the next cycle
void advance_bus_to_next_cycle(BusManager* manager);

// Function to request an action from the bus (sets the request operation and address)
void Enlist(BusRequestor* requestor, int addr, int BusActionType, BusManager* manager);

void write_next_cycle_of_bus(BusManager* manager);

void finish_bus_enlisting(BusManager* manager);

#endif