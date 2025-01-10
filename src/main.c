#include <stdio.h>
#include "main.h"


int main() {
    // Test MainMemory
    printf("Testing MainMemory...\n");
    MainMemory* memory = create_main_memory();

    // Write and read test
    unsigned int test_address = 1024;
    unsigned int test_data = 42;
    main_memory_write(memory, test_address, test_data);
    unsigned int read_data = main_memory_read(memory, test_address);

    if (read_data == test_data) {
        printf("MainMemory test passed: Address %u contains %u\n", test_address, read_data);
    }
    else {
        printf("MainMemory test failed: Address %u contains %u, expected %u\n", test_address, read_data, test_data);
    }

    // Clean up MainMemory
    main_memory_destroy(memory);

    // Test Core
    printf("Testing Core...\n");
    Core* core = core_create(1);

    // Example: Set some registers and verify
    core->registers_now[0] = 10;
    core->registers_now[1] = 20;
    printf("Core registers test: R0 = %u, R1 = %u\n", core->registers_now[0], core->registers_now[1]);

    // Example: Instruction memory test
    core->InstructionMemory[0] = 12345;
    printf("Core InstructionMemory test: IM[0] = %u\n", core->InstructionMemory[0]);

    // Clean up Core
    core_destroy(core);

    BusRequestor* req1 = bus_requestor_create(0);
    BusRequestor* req2 = bus_requestor_create(1);
    BusRequestor* req3 = bus_requestor_create(2);
    BusRequestor* req4 = bus_requestor_create(3);

    // Set initial priorities
    req1->priority = 1;
    req2->priority = 3;
    req3->priority = 2;
    req4->priority = 0;

    // Step 2: Create array of BusRequestors
    BusRequestor* requestors[NUM_REQUESTORS] = { req1, req2, req3, req4 };

    // Step 3: Create BusManager
    BusManager* manager = bus_manager_create(requestors);
    manager->bus_origid = 0; // Set origin requestor index (e.g., req2)
   

    // Step 4: Test arrangePriorities
    printf("Before arrangePriorities:\n");
    for (int i = 0; i < NUM_REQUESTORS; ++i) {
        printf("Requestor %d: Priority %d\n", manager->requestors[i]->id, manager->requestors[i]->priority);
    }

    arrangePriorities(manager);

    printf("\nAfter arrangePriorities:\n");
    for (int i = 0; i < NUM_REQUESTORS; ++i) {
        printf("Requestor %d: Priority %d\n", manager->requestors[i]->id, manager->requestors[i]->priority);
    }

    RequestForBus(manager, req4);

    printf("\nAfter RequestForBus:\n");
    for (int i = 0; i < NUM_REQUESTORS; ++i) {
        if (manager->enlisted_requestors[i]) {
            printf("Enlisted Requestor %d\n", manager->enlisted_requestors[i]->id);
        }
    }

    // Step 6: Destroy BusRequestors and BusManager
    bus_requestor_destroy(req1);
    bus_requestor_destroy(req2);
    bus_requestor_destroy(req3);
    bus_requestor_destroy(req4);
    bus_manager_destroy(manager);
}

    
