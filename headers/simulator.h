#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <stdio.h>

#include "main_memory.h"
#include "bus_snooper.h"
#include "bus_requestor.h"
#include "cache_block.h"
#include "core.h"
#include "constants.h"
#include "decode_stage.h"
#include "memory_stage.h"
#include "execute_stage.h"
#include "instruction.h"
#include "file_handler.h"

void run_simulator(int argc, char* argv[]);

#endif