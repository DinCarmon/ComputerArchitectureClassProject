#include "simulator.h"
#include "bus_manager.h"

void run_simulator(int argc, char* argv[])
{
    // 1. Create all neccessary objects

    // Arrays for the per-core files
    FILE* instruction_memory_files[NUM_OF_CORES] = { NULL };
    FILE* register_files[NUM_OF_CORES] = { NULL };
    FILE* core_trace_files[NUM_OF_CORES] = { NULL };
    FILE* data_cache_files[NUM_OF_CORES] = { NULL };
    FILE* status_cache_files[NUM_OF_CORES] = { NULL };
    FILE* stats_files[NUM_OF_CORES] = { NULL };

    // Pointers for single-file streams
    FILE* main_memory_input_file = NULL;
    FILE* main_memory_output_file = NULL;
    FILE* bus_trace_file = NULL;

    // Create the cores
    Core cores[NUM_OF_CORES];

    // Creating the main memory for the sim
    MainMemory main_memory = create_main_memory();

    // Create bus manager
    BusManager manager = bus_manager_create(&cores, &main_memory);

    // 2. Initialize all parameters, reset cache memories, Load main memory and cores memory

    get_all_file_descriptors(argc, argv,
                             instruction_memory_files,
                             &main_memory_input_file,
                             &main_memory_output_file,
                             register_files,
                             core_trace_files,
                             &bus_trace_file,
                             data_cache_files,
                             status_cache_files,
                             stats_files);
    
    for (int i = 0; i < NUM_OF_CORES; i++)
    {
        cores[i] = core_create(i + 1);
    }

    

    // load main memory
    loadMainMemory(main_memory_input_file, &main_memory);

    

    // load all cores i memory
    loadCoresImemory(&cores, instruction_memory_files);

	//need to add corePipeLine creation


    (void)manager;
}