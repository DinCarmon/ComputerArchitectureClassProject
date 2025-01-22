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
        instructionMemoryFiles,
        &mainMemoryInputFile,
        &mainMemoryOutputFile,
        registerFiles,
        coreTraceFiles,
        &busTraceFile,
        dataCacheFiles,
        statusCacheFiles,
        statsFiles);

    ////create an array for the cores in the simulator
    Core cores[NUM_OF_CORES];
    BusRequestor* requestors[NUM_OF_CORES];
    for (int i = 0; i < NUM_OF_CORES; i++)
    {
        cores[i] = core_create(i + 1);
        requestors[i] = &(cores[i].requestor);
    }

    ////creating the main memory for the sim
    MainMemory main_memory = create_main_memory();

    //// load main memory
    loadMainMemory(mainMemoryInputFile, &main_memory);

    //// create bus manager
    BusManager manager = bus_manager_create(requestors, &cores, &main_memory);

    //// load all cores i memory
    loadCoresImemory(&cores, instructionMemoryFiles);

	//need to add corePipeLine creation


    (void)manager;
}