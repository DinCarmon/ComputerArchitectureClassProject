#pragma once
// Include all the necessary headers
#include "main_memory.h"
#include "bus_manager.h"
#include "bus_snooper.h"
#include "bus_requestor.h"
//#include "bus_request.h"
#include "cache.h"
#include "core.h"
#include "defines.h"
#include <stdio.h>
#include "constants.h"
#include "decode_stage.h"
#include "fetch_stage.h"
#include "memory_stage.h"
#include "execute_stage.h"
#include "instruction.h"
#include "file_handler.h"


// the simulator start point and logic
int main(int argc, char* argv[]);
