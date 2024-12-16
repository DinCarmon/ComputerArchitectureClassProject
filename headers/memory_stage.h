#pragma once

#include <stdbool.h>  // For the bool type
// Define the memoryStage struct

typedef struct {
    stageData data;                     // Nested stageData struct
    bool (*run_operation)(void);        // Function pointer for operations
} memoryStage;

// Declare an example function for run_operation
bool memory_stage_run_operation(void);

