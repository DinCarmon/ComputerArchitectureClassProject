#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>

#include "main_memory.h"
#include "bus_manager.h"
#include "bus_snooper.h"
#include "bus_requestor.h"
#include "cache_block.h"
#include "core.h"
#include "constants.h"
#include "decode_stage.h"
#include "fetch_stage.h"
#include "memory_stage.h"
#include "execute_stage.h"
#include "instruction.h"
#include "file_handler.h"


// the simulator start point and logic
int main(int argc, char* argv[]);

#endif