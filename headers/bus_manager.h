#pragma once
#include "defines.h"
#include "bus_requestor.h"
#include "main_memory.h"
#include "cache.h"



typedef struct {
    uint32_t LastTransactionCycle;  // The last cycle a transaction occurred
    FlipFlop_int32_t BusStatus;   // status of the bus
    FlipFlop_bool Interupted;       // is the bus on interrupted mode
    int32_t interuptor_id;          // id of the interupting core 
    bool bus_shared;           // True if the bus is in shared mode
    uint32_t bus_origid;            // Originating ID for the current transaction
    uint32_t bus_cmd;               // Command being executed on the bus
    uint32_t bus_addr;              // Address involved in the bus transaction
    uint32_t bus_data;              // Data being transferred on the bus
    uint32_t core_turn;             // Core that has the current turn
    uint32_t numOfCyclesInSameStatus; // cycels in one transaction
    BusRequestor* requestors[NUM_REQUESTORS]; // Fixed array of BusRequestors
    BusRequestor* enlisted_requestors[NUM_REQUESTORS]; // List of enlisted BusRequestors
    Cache* caches[NUM_REQUESTORS];     // fiexed array of caches
    MainMemory* main_memory;             // pointer to the main memory
} BusManager;

// Function to create and initialize a BusManager
BusManager* bus_manager_create(BusRequestor** requestors, Cache** chaches, MainMemory* main_memory);

// Function to release manager memory
void bus_manager_destroy(BusManager* manager);

// Function to reset the BusManager (e.g., after a transaction)
void bus_manager_reset(BusManager* manager);

// Function to check if the bus is free
bool IsBusFree(const BusManager* manager);

// Function to arrange the priorities of the requestors based on the bus_origid
void arrangePriorities(BusManager* manager);

// Function to add a BusRequestor to the enlisted_requestors list
void RequestForBus(BusManager* manager, BusRequestor* requestor);

// Function to reset the enlisted_requestors list to zero
void resetEnlistedRequestors(BusManager* manager);

// Function to assign the core with the lowest priority to core_turn
void FinishBusEnlisting(BusManager* manager);

// Function to write to the bus line after a requestor is chosen
void WriteBusLines(BusManager* manager, BusRequestor* requestor);

// Function to write to the bus data line
void writeBusData(BusManager* manager, int32_t diff, bool read, Cache* cache);

// Function to advance the bus to the next cycle
void AdvanceBusToNextCycle(BusManager* manager, int currentCycle, bool KeepValue);

// Function to get the state that needed to be put in the cache
uint32_t StateToUpdate(BusManager* manager, Cache* cache);