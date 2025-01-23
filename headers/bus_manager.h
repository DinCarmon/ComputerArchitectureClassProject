#ifndef BUS_MANAGER_H
#define BUS_MANAGER_H

#include "bus_requestor.h"
#include "main_memory.h"
#include "constants.h"
#include "cache_block.h"
#include "flip_flop.h"

struct core;

typedef struct busManager {
    uint32_t last_transaction_on_bus_cycle;             // The last cycle a transaction occurred (i.e. there was something on the bus).
    FlipFlop_BusStatus bus_status;                      // status of the bus
    bool interrupted_by_another_snooper;                // If another cache wishes to answer.
    int32_t interuptor_id;                              // id of the interupting core 
    FlipFlop_BusShareStatus bus_shared;                 // True if the bus is in shared mode
    FlipFlop_int32_t bus_origid;                        // Originating ID for the current transaction
    FlipFlop_BusCmd bus_cmd;                            // Command being executed on the bus
    FlipFlop_uint32_t bus_data;                         // Data being transferred on the bus.
    FlipFlop_uint32_t bus_line_addr;                    // the address that will go on the bus lines
    FlipFlop_int32_t core_turn;                         // Core that has the current turn
    BusRequestor* requestors[NUM_OF_CORES];             // Fixed array of BusRequestors
    BusRequestor* enlisted_requestors[NUM_OF_CORES];    // List of enlisted BusRequestors
    MainMemory* main_memory;                            // pointer to the main memory

    uint64_t* p_cycle;
} BusManager;

// Function to create and initialize a BusManager
void configure_bus_manager(BusManager* manager, struct core** cores, MainMemory* main_memory);

// function to call when bus needs to be interupted (state m and busrd/busrdx)
void interrupt_bus(BusManager* manager, int32_t interruptor_id);

// Function to check if we can start a new transaction at the next cycle.
bool is_open_for_start_of_new_enlisting(BusManager* manager);

// Function to advance the bus to the next cycle
void advance_bus_to_next_cycle(BusManager* manager);

// Function to request an action from the bus (sets the request operation and address)
void enlist_to_bus(BusRequestor* requestor, int addr, BusCmd BusActionType, BusManager* manager);

void write_next_cycle_of_bus(BusManager* manager);

void finish_bus_enlisting(BusManager* manager);

#endif