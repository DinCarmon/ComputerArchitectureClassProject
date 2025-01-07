#include <stdio.h>
#include "main_memory.h"
#include "bus_manager.h"
#include "bus_snooper.h"
#include "cache.h"
#include "core.h"

int main() {
    // Initialize main memory
    MainMemory memory = main_memory_create();
    printf("Main memory initialized.\n");

    // Initialize bus manager
    BusManager manager = bus_manager_create();
    printf("Bus manager initialized.\n");

    // Initialize caches (for testing, we'll use 2 caches)
    Cache cache1, cache2;
    cache1.size = 16; // Example cache size
    cache2.size = 16;
    printf("Caches initialized.\n");

    // Initialize cores
    Core core1 = core_create(1);
    Core core2 = core_create(2);
    printf("Cores initialized.\n");

    // Initialize bus snoopers for each core
    BusSnooper snooper1 = bus_snooper_create(&core1, &manager, 1);
    BusSnooper snooper2 = bus_snooper_create(&core2, &manager, 2);
    printf("Bus snoopers initialized.\n");

    // Simulate a bus transaction (BUS_RD)
    manager.bus_cmd = BUS_RD;
    manager.bus_addr = 0xA;  // Example address
    manager.bus_origid = 1; // Core 1 originates this request
    printf("Simulated bus transaction: BUS_RD, address: 0xA, originating ID: 1.\n");

    // Core 2 reacts to the bus transaction
    snoop(&snooper2, &cache2);
    printf("Core 2 snooped the bus.\n");

    // Verify cache state for Core 2
    int state = getState(0xA, &cache2);
    printf("Cache state for address 0xA in Core 2: %d\n", state);

    // Simulate a BUS_RDX transaction from Core 2
    manager.bus_cmd = BUS_RDX;
    manager.bus_addr = 0xB;  // Another example address
    manager.bus_origid = 2; // Core 2 originates this request
    printf("Simulated bus transaction: BUS_RDX, address: 0xB, originating ID: 2.\n");

    // Core 1 reacts to the bus transaction
    snoop(&snooper1, &cache1);
    printf("Core 1 snooped the bus.\n");

    // Check if address 0xB in Core 1 is invalidated
    state = getState(0xB, &cache1);
    printf("Cache state for address 0xB in Core 1: %d\n", state);

    // Simulate writing data to memory
    unsigned int data = 42; // Example data
    main_memory_write(&memory, 0xC, data);
    printf("Wrote data %u to memory at address 0xC.\n", data);

    // Simulate reading data from memory
    unsigned int read_data = main_memory_read(&memory, 0xC);
    printf("Read data from memory at address 0xC: %u\n", read_data);

    // Advance the bus manager to the next cycle
    AdvanceBusToNextCycle(&manager);
    printf("Bus manager advanced to the next cycle.\n");

    return 0;
}
