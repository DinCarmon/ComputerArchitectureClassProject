#include "main.h"
#include <stdlib.h>


int main(int argc, char* argv[]) {
    // Arrays for the per-core files
    FILE* instructionMemoryFiles[NUM_OF_CORES] = { NULL };
    FILE* registerFiles[NUM_OF_CORES] = { NULL };
    FILE* coreTraceFiles[NUM_OF_CORES] = { NULL };
    FILE* dataCacheFiles[NUM_OF_CORES] = { NULL };
    FILE* statusCacheFiles[NUM_OF_CORES] = { NULL };
    FILE* statsFiles[NUM_OF_CORES] = { NULL };

    // Pointers for single-file streams
    FILE* mainMemoryInputFile = NULL;
    FILE* mainMemoryOutputFile = NULL;
    FILE* busTraceFile = NULL;


    //get all file descriptors on write mode
    getAllFileDescritpors(argc, argv,
        instructionMemoryFiles[NUM_OF_CORES],
        mainMemoryInputFile,
        mainMemoryOutputFile,
        registerFiles[NUM_OF_CORES],
        coreTraceFiles[NUM_OF_CORES],
        busTraceFile,
        dataCacheFiles[NUM_OF_CORES],
        statusCacheFiles[NUM_OF_CORES],
        statsFiles[NUM_OF_CORES]);

    ////create an array for the cores in the simulator
    //Core** cores = create_cores_array(NUM_OF_CORES);

    //// Create an array of pointers to those existing requestors
    //BusRequestor** requestors = create_busrequestors_array(cores, NUM_OF_CORES);

    ////creating the main memory for the sim
    //MainMemory* main_memory = create_main_memory();

    //// load main memory
    //loadMainMemory(mainMemoryInputFile, main_memory);

    //// create bus manager
    //BusManager* manager = bus_manager_create(requestors, cores, main_memory);

    //// load all cores i memory
    //loadCoresImemory(cores, instructionMemoryFiles);

    //

}