#include "core_pipeline.h"


// Function to initialize a Core instance on the heap
CorePipeLine pipe_create(int id) {
    // Allocate memory for the Core structure
    CorePipeLine newPipe;

    // Initialize fields

    newPipe.decode_stage = createDecodeStage();
    newPipe.memory_stage = createMemoryStage();
    newPipe.execute_stage = createExecuteStage();
    newPipe.fetch_stage = createFetchStage();
    newPipe.writeback_stage = createWriteBackStage();

    return newPipe;
}
