#include "core.h"


// Function to initialize a Core instance on the heap
Core* core_create(int id) {
    // Allocate memory for the Core structure
    Core* newCore = (Core*)malloc(sizeof(Core));
    if (newCore == NULL) {
        fprintf(stderr, "Memory allocation failed for Core\n");
        exit(EXIT_FAILURE);
    }

    // Initialize fields
    newCore->id = id;
    newCore->pc_register.now = 0;
    newCore->pc_register.updated = 0;
    memset(newCore->registers, 0, sizeof(newCore->registers));
    memset(newCore->InstructionMemory, 0, sizeof(newCore->InstructionMemory));

    // Allocate memory for caches
    newCore->cache_now = (Cache*)malloc(sizeof(Cache));
    newCore->cache_updated = (Cache*)malloc(sizeof(Cache));

    if (newCore->cache_now == NULL || newCore->cache_updated == NULL) {
        fprintf(stderr, "Memory allocation failed for caches\n");
        free(newCore); // Free the Core structure if cache allocation fails
        exit(EXIT_FAILURE);
    }

    // Initialize the caches
    *newCore->cache_now = cache_create();
    *newCore->cache_updated = cache_create();
    newCore->decode_stage = createDecodeStage();
    newCore->decode_stage.state->myCore = newCore;
    newCore->memory_stage = createMemoryStage();
    newCore->memory_stage.state->myCore = newCore;
    newCore->execute_stage = createExecuteStage();
    newCore->execute_stage.state->myCore = newCore;
    newCore->fetch_stage = createFetchStage();
    newCore->fetch_stage.state->myCore = newCore;
    newCore->writeback_stage = createWriteBackStage();
    newCore->writeback_stage.state->myCore = newCore;

    newCore->requestor = bus_requestor_create(newCore->id);
    newCore->snooper = bus_snooper_create(newCore->id); 

    return newCore;
}

// function to create an arry of core pointers
Core** create_cores_array(int numCores)
{
    // Allocate an array of Core* (pointers to Core)
    Core** cores = (Core**)malloc(sizeof(Core*) * numCores);
    if (cores == NULL) {
        fprintf(stderr, "Failed to allocate array of Core pointers.\n");
        exit(EXIT_FAILURE);
    }

    // For each index, create an individual Core on the heap
    for (int i = 0; i < numCores; i++) {
        cores[i] = core_create(i+1);
    }

    return cores; // Return the array of pointers
}


// Function to destroy a Core instance and free all memory
void core_destroy(Core* core) {
    if (core == NULL) {
        return; // Do nothing if the pointer is NULL
    }

    // Free allocated memory for caches
    free(core->cache_now);
    free(core->cache_updated);

    // Free the Core structure itself
    free(core);
}


// Destroys an entire array of Core pointers and the array itself
void destroy_cores_array(Core** cores, int numCores)
{
    if (cores == NULL) {
        return;
    }

    for (int i = 0; i < numCores; i++) {
        // core_destroy is the function above
        if (cores[i]) {
            core_destroy(cores[i]);
            cores[i] = NULL;
        }
    }

    // Now free the array of pointers
    free(cores);
    cores = NULL;
}

// Function to advance the core to the next cycle
void advance_core(Core* core, bool keep_value) {
    UPDATE_FLIP_FLOP(core->pc_register, keep_value);
    for (int i = 0; i < 32; i++) {
        UPDATE_FLIP_FLOP(core->registers[i], keep_value);
    }
    UPDATE_FLIP_FLOP(core->snooper->busSnooperActive);
    UPDATE_FLIP_FLOP(core->requestor->IsRequestOnBus);
    UPDATE_FLIP_FLOP(core->requestor->IsAlreadyAskedForBus);
    UPDATE_FLIP_FLOP(core->requestor->LastCycle);

    core->cache_now = core->cache_updated;
}

/**
 * Creates and returns a new array of BusRequestor* by gathering
 * them from each Core in `cores`.
 */
BusRequestor** create_busrequestors_array(Core** cores, int numCores)
{
    // Validate input
    if (!cores || numCores <= 0) {
        return NULL;
    }

    // Allocate an array of pointers to BusRequestor
    BusRequestor** requestors = malloc(sizeof(BusRequestor*) * numCores);
    if (!requestors) {
        perror("Failed to allocate requestors array");
        return NULL;
    }

    // Populate each element with the requestor from each Core
    for (int i = 0; i < numCores; i++) {
        if (cores[i]) {
            requestors[i] = cores[i]->requestor;
        }
        else {
            requestors[i] = NULL;  // or handle error as needed
        }
    }

    return requestors;
}


/**
 * Frees the dynamically allocated array of BusRequestor*.
 * This does NOT free the BusRequestors themselves (they
 * belong to their respective Core).
 *
 * @param requestors  The array of BusRequestor* to free.
 */
void destroy_busrequestors_array(BusRequestor** requestors)
{
    if (requestors == NULL) {
        return; // Nothing to free
    }
    free(requestors);
}


