#pragma once
#include <stdbool.h>
#include "bus_requestor.h"
#include <limits.h> // For INT_MAX

#define NUM_REQUESTORS 4  // Fixed number of BusRequestors

// Define bus status constants in uppercase
#define BUS_FREE   1    // The bus is free and available for use
#define BUS_RD     2    // The bus is performing a read operation
#define BUS_RDX    3    // The bus is performing a read-exclusive operation
#define BUS_INVALID 4   // The bus is in an invalid state (e.g., error state or not in use)

typedef struct {
    int LastTransactionCycle;  // The last cycle a transaction occurred
    int BusStatus;             // Current status of the bus
    bool bus_shared;           // True if the bus is in shared mode
    int bus_origid;            // Originating ID for the current transaction
    int bus_cmd;               // Command being executed on the bus
    int bus_addr;              // Address involved in the bus transaction
    int bus_data;              // Data being transferred on the bus
    int core_turn;             // Core that has the current turn
    BusRequestor requestors[NUM_REQUESTORS]; // Fixed array of BusRequestors
    BusRequestor enlisted_requestors[NUM_REQUESTORS]; // List of enlisted BusRequestors
} BusManager;

// Function to create and initialize a BusManager
BusManager bus_manager_create(BusRequestor requestors[NUM_REQUESTORS]);

// Function to reset the BusManager (e.g., after a transaction)
void bus_manager_reset(BusManager* manager);

// Function to check if the bus is free
bool IsBusFree(const BusManager* manager);

// Function to arrange the priorities of the requestors based on the bus_origid
void arrangePriorities(BusManager* manager);

// Function to add a BusRequestor to the enlisted_requestors list
void RequestForBus(BusManager* manager, BusRequestor requestor);

// Function to reset the enlisted_requestors list to zero
void resetEnlistedRequestors(BusManager* manager);

// Function to assign the core with the lowest priority to core_turn
void FinishBusEnlisting(BusManager* manager);
