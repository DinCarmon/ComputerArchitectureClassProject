#ifndef CORE_PIPELINE
#define CORE_PIPELINE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "flip_flop.h"
#include "writeback_stage.h"
#include "fetch_stage.h"
#include "decode_stage.h"
#include "execute_stage.h"
#include "memory_stage.h"



// Define the Core struct
typedef struct {
    int id;                          //core id
    WriteBackStage writeback_stage;
    ExecuteStage execute_stage;
    DecodeStage decode_stage;
    FetchStage fetch_stage;
    MemoryStage memory_stage;
} CorePipeLine;

// Function to initialize a Core instance
CorePipeLine pipe_create(int id);

#endif