#include "simulator.h"
#include "bus_manager.h"

void initialize_simulator_objects(int argc, char* argv[],
                                  FILE* instruction_memory_files[NUM_OF_CORES],
                                  FILE* register_files[NUM_OF_CORES],
                                  FILE* core_trace_files[NUM_OF_CORES],
                                  FILE* data_cache_files[NUM_OF_CORES],
                                  FILE* status_cache_files[NUM_OF_CORES],
                                  FILE* stats_files[NUM_OF_CORES],
                                  FILE* main_memory_input_file,
                                  FILE* main_memory_output_file,
                                  FILE* bus_trace_file,
                                  Core cores[NUM_OF_CORES],
                                  MainMemory* main_memory,
                                  BusManager* bus_manager)
{
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
        configure_core(&(cores[i]), i, bus_manager);
    }

    // load main memory
    reset_main_memory(main_memory);
    load_main_memory_from_file(main_memory_input_file, main_memory);

    configure_bus_manager(bus_manager, (struct core**)cores, main_memory);

    // load all cores i memory
    load_cores_instruction_memory_from_files(cores, instruction_memory_files);
}

void deploy_simulator(FILE* core_trace_files[NUM_OF_CORES],
                      FILE* bus_trace_file,
                      Core cores[NUM_OF_CORES],
                      BusManager* bus_manager)
{
    uint64_t cycle = 1;

    uint64_t last_succesful_writeback_execution[NUM_OF_CORES];
    uint64_t last_succesful_memory_execution[NUM_OF_CORES];
    uint64_t last_succesful_execute_execution[NUM_OF_CORES];
    uint64_t last_succesful_decode_execution[NUM_OF_CORES];
    uint64_t last_succesful_fetch_execution[NUM_OF_CORES];
    uint64_t last_insuccesful_memory_execution[NUM_OF_CORES];
    uint64_t last_insuccesful_decode_execution[NUM_OF_CORES];

    // Update cores and bus manager with the cycle instance and initiate simulator meta parameters
    bus_manager->p_cycle = &cycle;
    for(int i = 0; i < NUM_OF_CORES; i++)
    {
        cores[i].p_cycle = &cycle;
        last_succesful_writeback_execution[i] = 0;
        last_succesful_memory_execution[i] = 0;
        last_succesful_execute_execution[i] = 0;
        last_succesful_decode_execution[i] = 0;
        last_succesful_fetch_execution[i] = 0;
        last_insuccesful_memory_execution[i] = 0;
        last_insuccesful_decode_execution[i] = 0;
    }

    while(true)
    {
        // 1. Check ending condition for simulation
        bool end_simulation = true;
        for (int i = 0; i < NUM_OF_CORES; i++)
        {
            if (cores[i].writeback_stage.state.inputState.instruction.opcode != Halt)
            {
                end_simulation = false;
            }
        }
        if (end_simulation == true)
        {
            break;
        }

        // 2. Do operations of each core
        for(int i = 0; i < NUM_OF_CORES; i++)
        {
            // Ensure every stage happends when not stalled, and has some input.
            // This is crucial for correct statistics on core.

            if (cycle >= 5 &&
                last_succesful_memory_execution[i] >= last_succesful_writeback_execution[i])
            {
                do_write_back_operation(&(cores[i].writeback_stage));
                last_succesful_writeback_execution[i] = cycle;
            }

            if (cycle >= 4 &&
                last_succesful_execute_execution[i] >= last_succesful_memory_execution[i])
            {
                bool memory_should_stall = do_memory_operation(&(cores[i].memory_stage));
                if (memory_should_stall)
                    last_succesful_memory_execution[i] = cycle;   
                else
                    last_insuccesful_memory_execution[i] = cycle;
            }

            if (cycle >= 3 &&
                last_insuccesful_memory_execution[i] != cycle - 1 &&
                last_succesful_decode_execution[i] >= last_succesful_execute_execution[i])
            {
                do_execute_operation(&(cores[i].execute_stage));
                last_succesful_execute_execution[i] = cycle;
            }

            if (cycle >= 2 &&
                last_insuccesful_memory_execution[i] != cycle - 1 &&
                last_succesful_fetch_execution[i] >= last_succesful_decode_execution[i])
            {
                bool decode_should_stall = do_decode_operation(&(cores[i].decode_stage));
                if (decode_should_stall)
                    last_succesful_decode_execution[i] = cycle;
                else
                    last_insuccesful_decode_execution[i] = cycle;   
            }
            
            if (cycle >= 1 &&
                last_insuccesful_memory_execution[i] != cycle - 1 &&
                last_insuccesful_decode_execution[i] != cycle -1)
            {
                do_fetch_operation(&(cores[i].fetch_stage));
                last_succesful_fetch_execution[i] = cycle;
            }
        }

        if (is_open_for_start_of_new_enlisting(bus_manager))
            finish_bus_enlisting(bus_manager);

        for(int i = 0; i < NUM_OF_CORES; i++)
        {
            do_snoop_operation(&(cores[i].snooper));
        }

        write_next_cycle_of_bus(bus_manager);
        
        // 3. Log everthing
        for (int i = 0; i < NUM_OF_CORES; i++)
        {
            if (cores[i].writeback_stage.state.inputState.instruction.opcode != Halt)
                write_core_trace_line(core_trace_files[i],
                                        &(cores[i]),
                                        last_succesful_writeback_execution[i],
                                        last_succesful_memory_execution[i],
                                        last_succesful_execute_execution[i],
                                        last_succesful_decode_execution[i],
                                        last_succesful_fetch_execution[i],
                                        last_insuccesful_memory_execution[i],
                                        last_insuccesful_decode_execution[i]);
        }
        write_bus_trace_line(bus_trace_file, bus_manager);

        // 4. Advance all flip flops, and cycle
        cycle++;
        for(int i = 0; i < NUM_OF_CORES; i++)
        {
            advance_core(&(cores[i]), last_succesful_writeback_execution[i],
                                      last_succesful_memory_execution[i],
                                      last_succesful_execute_execution[i],
                                      last_succesful_decode_execution[i],
                                      last_succesful_fetch_execution[i],
                                      last_insuccesful_memory_execution[i],
                                      last_insuccesful_decode_execution[i]);
        }
        advance_bus_to_next_cycle(bus_manager);
    }
}

void write_simulator_summary_files(FILE* register_files[NUM_OF_CORES],
                                   FILE* data_cache_files[NUM_OF_CORES],
                                   FILE* status_cache_files[NUM_OF_CORES],
                                   FILE* stats_files[NUM_OF_CORES],
                                   FILE* main_memory_output_file,
                                   Core cores[NUM_OF_CORES],
                                   MainMemory* main_memory)
{
    writeMainMemory(main_memory_output_file,
                    main_memory);
    
    for (int i = 0; i < NUM_OF_CORES; i++)
    {
        writeRegisterFile(register_files[i],
                          cores[i].registers);

        write_data_cache_file(data_cache_files[i],
                              &(cores[i].cache_now));

        write_status_cache_file(status_cache_files[i],
                                 &(cores[i].cache_now));

        write_stats_file(stats_files[i], &(cores[i]));
    }
}

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
    MainMemory main_memory;

    // Create bus manager
    BusManager bus_manager;

    // 2. Initialize all parameters, reset cache memories, Load main memory and cores memory
    initialize_simulator_objects(argc, argv,
                                 instruction_memory_files,
                                 register_files,
                                 core_trace_files,
                                 data_cache_files,
                                 status_cache_files,
                                 stats_files,
                                 main_memory_input_file,
                                 main_memory_output_file,
                                 bus_trace_file,
                                 cores,
                                 &main_memory,
                                 &bus_manager);

    // 3. Run simulator
    deploy_simulator(core_trace_files,
                     bus_trace_file,
                     cores,
                     &bus_manager);

    // 4. Write run summary files
    write_simulator_summary_files(register_files,
                                  data_cache_files,
                                  status_cache_files,
                                  stats_files,
                                  main_memory_output_file,
                                  cores,
                                  &main_memory);
}