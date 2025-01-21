#include "core_pipeline.h"


// Function to initialize a Core instance on the heap
CorePipeLine* pipe_create(int id) {
    // Allocate memory for the Core structure
    CorePipeLine* newPipe = (CorePipeLine*)malloc(sizeof(CorePipeLine));
    if (newPipe == NULL) {
        fprintf(stderr, "Memory allocation failed for Corepipe\n");
        exit(EXIT_FAILURE);
    }

    // Initialize fields

    newPipe->decode_stage = createDecodeStage();
    newPipe->memory_stage = createMemoryStage();
    newPipe->execute_stage = createExecuteStage();
    newPipe->fetch_stage = createFetchStage();
    newPipe->writeback_stage = createWriteBackStage();

    return newPipe;
}

// Function to destroy a Core instance and free all memory
void pipe_destroy(Core* core) {
    if (core == NULL) {
        return; // Do nothing if the pointer is NULL
    }


    // Free the Core structure itself
    free(core);
}