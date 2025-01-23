#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "constants.h"
#include "core.h"
#include "main_memory.h"
#include "file_handler.h"
#include "bus_manager.h"

FILE* openFile(char* filePath, char* mode)
{
    FILE *file = fopen(filePath, mode);
    if (file == NULL)
    {
        char errorMessage[MAX_ERROR_MESSAGE_SIZE] = "Error opening file: ";
        strncat(errorMessage, filePath, sizeof(errorMessage) - 1);
        strncat(errorMessage, " in mode: ", sizeof(errorMessage) - 1);
        strncat(errorMessage, mode, 1);
        strncat(errorMessage, "", 1);
        perror(errorMessage);
    }

    return file;
}

bool isHexString(char* string)
{
    for (int i = 0; string[i] != '\0'; i++)
    {
        char digit = string[i];

        if (!((digit >= '0' && digit <= '9') ||
            (digit >= 'A' && digit <= 'F') ||
            (digit >= 'a' && digit <= 'f')))
        {
            return false;
        }
    }
    return true;
}

uint32_t hexStringToUint32(char* string)
{
    uint32_t num = 0;
    for (int i = 0; i < 8; i++)
    {
        char digit = string[i];

        uint8_t digitInt = 0;
        if (digit >= '0' && digit <= '9')
            digitInt = digit - '0';
        if (digit >= 'A' && digit <= 'F')
            digitInt = digit - 'A' + 10;

        num *= 16;
        num += digitInt;
    }

    return num;
}

bool readNumFromFileInHex(FILE* file, uint32_t* num)
{
    char line[11]; // 8 hex digits + /r + /n + null ending
    bool isNotEndOfFile = fgets(line, sizeof(line) - 1, file);
    line[8] = '\0'; // Get rid of /r/n

    // Check if its the end of the file
    if (!isNotEndOfFile)
    {
        *num = 0;
        return false;
    }
        
    if (!isHexString(line))
    {
        char errorMessage[MAX_ERROR_MESSAGE_SIZE] = "Not a hex string: ";
        strncat(errorMessage, line, sizeof(errorMessage) - 1);
        perror(errorMessage);
    }

    *num = hexStringToUint32(line);
    return true;
}

void numberToHexString(uint32_t num, char* str, uint32_t hexRepresentationLen)
{
    for (int i = hexRepresentationLen - 1; i >= 0; i--)
    {
        uint8_t res = num % 16;

        if (res >= 10)
            str[i] = 'A' + (res - 10);
        else
            str[i] = '0' + res;
        
        num /= 16;
    }
}

void writeNumberToFileInHex(FILE* file, uint32_t num)
{
    char line[10];
    line[8] = '\n';
    line[9] = '\0';

    numberToHexString(num, line, 8);

    fprintf(file, "%s", line);
}

void writeInstructionAddressToFile(FILE* file, uint32_t instructionAddress)
{
    char addressInHex[4];
    addressInHex[3] = '\0';
    
    numberToHexString(instructionAddress, addressInHex, 3);

    fprintf(file, "%s", addressInHex);
}

void get_all_file_descriptors(int argc,
                           char* argv[],
                           FILE* instruction_memory_files[NUM_OF_CORES],
                           FILE** main_memory_input_file,
                           FILE** main_memory_output_file,
                           FILE* register_files[NUM_OF_CORES],
                           FILE* core_trace_files[NUM_OF_CORES],
                           FILE** bus_trace_file,
                           FILE* data_cache_files[NUM_OF_CORES],
                           FILE* status_cache_files[NUM_OF_CORES],
                           FILE* stats_files[NUM_OF_CORES])
{
    if (argc == 2)
    {
        *main_memory_input_file = openFile(DEFAULT_MAIN_MEMORY_FILE_INPUT_PATH, "r");
        *main_memory_output_file = openFile(DEFAULT_MAIN_MEMORY_FILE_OUTPUT_PATH, "w");
        *bus_trace_file = openFile(DEFAULT_BUS_TRACE_PATH, "w");
        for (int i = 0; i < NUM_OF_CORES; i++)
        {
            char coreNumStr[MAX_PATH_SIZE] = "";


            
            snprintf(coreNumStr, sizeof(coreNumStr), "%d", i);

            char instructionMemoryFilePath[MAX_PATH_SIZE] = "";
            strncat(instructionMemoryFilePath, DEFAULT_INSTRUCTION_MEMORY_FILE_PATH_PREFIX, sizeof(instructionMemoryFilePath) - 1);
            strncat(instructionMemoryFilePath, coreNumStr, sizeof(instructionMemoryFilePath) - 1);
            strncat(instructionMemoryFilePath, ".txt", sizeof(instructionMemoryFilePath) - 1);
            instruction_memory_files[i] = openFile(instructionMemoryFilePath, "r");

            char registerFilePath[MAX_PATH_SIZE] = "";
            strncat(registerFilePath, DEFAULT_REGISTER_FILE_PATH_PREFIX, sizeof(registerFilePath) - 1);
            strncat(registerFilePath, coreNumStr, sizeof(registerFilePath) - 1);
            strncat(registerFilePath, ".txt", sizeof(registerFilePath) - 1);
            register_files[i] = openFile(registerFilePath, "w");

            char coreTraceFilePath[MAX_PATH_SIZE] = "";
            strncat(coreTraceFilePath, DEFAULT_CORE_TRACE_FILE_PATH_PREFIX, sizeof(coreTraceFilePath) - 1);
            strncat(coreTraceFilePath, coreNumStr, sizeof(coreTraceFilePath) - 1);
            strncat(coreTraceFilePath, DEFAULT_CORE_TRACE_FILE_PATH_SUFFIX, sizeof(coreTraceFilePath) - 1);
            core_trace_files[i] = openFile(coreTraceFilePath, "w");

            char dataCacheFilePath[MAX_PATH_SIZE] = "";
            strncat(dataCacheFilePath, DEFAULT_DSRAM_CACHE_FILE_PATH_PREFIX, sizeof(dataCacheFilePath) - 1);
            strncat(dataCacheFilePath, coreNumStr, sizeof(dataCacheFilePath) - 1);
            strncat(dataCacheFilePath, ".txt", sizeof(dataCacheFilePath) - 1);
            data_cache_files[i] = openFile(dataCacheFilePath, "w");

            char statusCacheFilePath[MAX_PATH_SIZE] = "";
            strncat(statusCacheFilePath, DEFAULT_TSRAM_CACHE_FILE_PATH_PREFIX, sizeof(statusCacheFilePath) - 1);
            strncat(statusCacheFilePath, coreNumStr, sizeof(statusCacheFilePath) - 1);
            strncat(statusCacheFilePath, ".txt", sizeof(statusCacheFilePath) - 1);
            status_cache_files[i] = openFile(statusCacheFilePath, "w");

            char statsFilePath[MAX_PATH_SIZE] = "";
            strncat(statsFilePath, DEFAULT_TSRAM_CACHE_FILE_PATH_PREFIX, sizeof(statsFilePath) - 1);
            strncat(statsFilePath, coreNumStr, sizeof(statsFilePath) - 1);
            strncat(statsFilePath, ".txt", sizeof(statsFilePath) - 1);
            stats_files[i] = openFile(statsFilePath, "w");
        }
    }
    else if (argc == 28)
    {
        *main_memory_input_file = openFile(argv[5], "r");
        *main_memory_output_file = openFile(argv[6], "w");
        *bus_trace_file = openFile(argv[15], "w");
        for (int i = 0; i < NUM_OF_CORES; i++)
        {
            instruction_memory_files[i] = openFile(argv[1 + i], "r");
            register_files[i] = openFile(argv[7 + i], "w");
            core_trace_files[i] = openFile(argv[11 + i], "w");
            data_cache_files[i] = openFile(argv[16 + i], "w");
            status_cache_files[i] = openFile(argv[20 + i], "w");
            stats_files[i] = openFile(argv[24 + i], "w");
        }
    }
    else
    {
        perror("Wrong number of arguments. \
                Either provide all file paths as decribed in the project description,\
                or provide no arguments at all");
    }
}

void load_instruction_memory(FILE* instructionMemoryFile, uint32_t* instructionMemory)
{
    int address = 0;
    while (address < INSTRUCTION_MEMORY_DEPTH)
    {
        uint32_t instruction = 0;
        bool notEndOfFile = readNumFromFileInHex(instructionMemoryFile, &instruction);

        // Fill the memory with 0 if there are no more lines in the file
        if (!notEndOfFile)
            instruction = 0;
        
        instructionMemory[address] = instruction;
        address++;
    }
}

void load_cores_instruction_memory_from_files(Core cores[NUM_OF_CORES], FILE* instruction_memory_files[NUM_OF_CORES]) {
    for (int i = 0; i < NUM_OF_CORES; i++)
    {
        load_instruction_memory(instruction_memory_files[i], cores[i].instruction_memory);
        fclose(instruction_memory_files[i]);
    }
}

void load_main_memory_from_file(FILE* mainMemoryFile, MainMemory* mem)
{
    uint32_t address = 0;
    while (address < MEMORY_DEPTH)
    {
        uint32_t data = 0;
        bool notEndOfFile = readNumFromFileInHex(mainMemoryFile, &data);

        // Fill the memory with 0 if there are no more lines in the file
        if (!notEndOfFile)
            data = 0;
        
        (*mem).memory[address] = data;
        address++;
    }
}

void writeMainMemory(FILE* mainMemoryFile, MainMemory* mem)
{
    uint32_t address = 0;
    while (address < MEMORY_DEPTH)
    {
        uint32_t data = (*mem).memory[address];

        writeNumberToFileInHex(mainMemoryFile, data);
        
        address++;
    }
}

void writeRegisterFile(FILE* registerFile, FlipFlop_uint32_t* registerArr)
{
    // Skip R0, R1
    for(int i = 2; i < NUM_REGISTERS_PER_CORE; i++)
    {
        writeNumberToFileInHex(registerFile, registerArr[i].now);
    }
}

void write_core_trace_line(FILE* coreTraceFile,
                           Core* core,
                           uint64_t last_succesful_writeback_execution,
                           uint64_t last_succesful_memory_execution,
                           uint64_t last_succesful_execute_execution,
                           uint64_t last_succesful_decode_execution,
                           uint64_t last_succesful_fetch_execution,
                           uint64_t last_insuccesful_memory_execution,
                           uint64_t last_insuccesful_decode_execution)
{
    // Write cycle
    char cycleStr[MAX_PATH_SIZE] = "";
    snprintf(cycleStr, sizeof(cycleStr), "%d", (int)*(core->p_cycle));
    fprintf(coreTraceFile, "%s", cycleStr);
    fprintf(coreTraceFile, " ");

    // If fetch was not executed in this cycle print ---
    if (last_succesful_fetch_execution != *(core->p_cycle))
        fprintf(coreTraceFile, "---");
    else
        writeInstructionAddressToFile(coreTraceFile,
                                      core->pc_register.now);
    fprintf(coreTraceFile, " ");

    // If decode was not executed in this cycle or
    // (decode was executed in this cycle but fetch not -> i.e a stall)
    // Then print a ---
    if (((last_succesful_decode_execution != *(core->p_cycle)) &&
         (last_insuccesful_decode_execution != *(core->p_cycle))) ||
        last_succesful_fetch_execution != *(core->p_cycle) ||
        *(core->p_cycle) <= 1)
        fprintf(coreTraceFile, "---");
    else
        writeInstructionAddressToFile(coreTraceFile,
                                      core->decode_stage.state.inputState.instructionAddress);
    fprintf(coreTraceFile, " ");

    if ((last_succesful_execute_execution != *(core->p_cycle)) ||
        *(core->p_cycle) <= 2)
        fprintf(coreTraceFile, "---");
    else
        writeInstructionAddressToFile(coreTraceFile,
                                      core->execute_stage.state.inputState.instructionAddress);
    fprintf(coreTraceFile, " ");

    // If memory was not executed in this cycle or
    // (memory was executed in this cycle but execute not -> i.e a stall)
    // Then print a ---
    if (((last_succesful_memory_execution != *(core->p_cycle)) &&
         (last_insuccesful_memory_execution != *(core->p_cycle))) ||
        last_succesful_execute_execution != *(core->p_cycle) ||
        *(core->p_cycle) <= 3)
        fprintf(coreTraceFile, "---");
    else
        writeInstructionAddressToFile(coreTraceFile,
                                      core->memory_stage.state.inputState.instructionAddress);
    fprintf(coreTraceFile, " ");

    if (last_succesful_writeback_execution != *(core->p_cycle) ||
        *(core->p_cycle) <= 4)
        fprintf(coreTraceFile, "---");
    else
        writeInstructionAddressToFile(coreTraceFile,
                                      core->writeback_stage.state.inputState.instructionAddress);
    fprintf(coreTraceFile, " ");

    for(int i = 2; i < NUM_REGISTERS_PER_CORE; i++)
    {
        char registerInHexTmpStr[9];
        registerInHexTmpStr[8] = '\0';
        numberToHexString(core->registers[i].now, registerInHexTmpStr, 8);

        fprintf(coreTraceFile, " ");
        fprintf(coreTraceFile, "%s", registerInHexTmpStr);
    }

    fprintf(coreTraceFile, "\n");
}

void write_bus_trace_line(FILE* bus_trace_file,
                          BusManager* manager)
{
    // Write cycle
    char cycleStr[MAX_PATH_SIZE] = "";
    snprintf(cycleStr, sizeof(cycleStr), "%d", (int)*(manager->p_cycle));
    fprintf(bus_trace_file, "%s", cycleStr);
    fprintf(bus_trace_file, " ");

    char bus_origin_id[2];
    bus_origin_id[1] = '\0';
    int tmp = manager->bus_origid.now;
    if (tmp == -1) // I.E no one is writing to the bus
    {
        tmp = 0; // In such a case write 0 to the bus...
    }
    numberToHexString(tmp, bus_origin_id, 1);
    fprintf(bus_trace_file, "%s", bus_origin_id);
    fprintf(bus_trace_file, " ");

    char bus_cmd[2];
    bus_cmd[1] = '\0';
    numberToHexString(manager->bus_cmd.now, bus_cmd, 1);
    fprintf(bus_trace_file, "%s", bus_cmd);
    fprintf(bus_trace_file, " ");

    char bus_addr[6];
    bus_addr[5] = '\0';
    numberToHexString(manager->bus_line_addr.now, bus_addr, 5);
    fprintf(bus_trace_file, "%s", bus_addr);
    fprintf(bus_trace_file, " ");

    char bus_data[9];
    bus_data[8] = '\0';
    numberToHexString(manager->bus_data.now, bus_data, 8);
    fprintf(bus_trace_file, "%s", bus_data);
    fprintf(bus_trace_file, " ");

    char bus_shared[2];
    bus_shared[1] = '\0';
    numberToHexString(manager->bus_shared.now, bus_shared, 1);
    fprintf(bus_trace_file, "%s", bus_shared);

    fprintf(bus_trace_file, "\n");
}

void write_data_cache_file(FILE* cache_file,
                      Cache* cache)
{
    for(int i = 0; i < DATA_CACHE_WORD_DEPTH; i++)
    {
        writeNumberToFileInHex(cache_file, cache->dsram[i]);
    }
}

void write_status_cache_file(FILE* cache_file,
                             Cache* cache)
{
    for(int i = 0; i < DATA_CACHE_NUM_OF_BLOCKS_IN_CACHE; i++)
    {
        uint32_t word = (cache->tsram[i].state << TAG_FIELD_SIZE_IN_BITS) + cache->tsram[i].tag;
        writeNumberToFileInHex(cache_file, word);
    }
}

void write_stats_file(FILE* stats_file,
                      Core* core)
{
    fprintf(stats_file, "%s", "cycles ");
    fprintf(stats_file, "%d", (int)(core->halt_cycle));
    fprintf(stats_file, "%s", "\n");

    fprintf(stats_file, "%s", "instructions ");
    fprintf(stats_file, "%d", (int)(core->num_of_instructions_executed));
    fprintf(stats_file, "%s", "\n");

    fprintf(stats_file, "%s", "read_hit ");
    fprintf(stats_file, "%d", (int)(core->num_read_hits));
    fprintf(stats_file, "%s", "\n");

    fprintf(stats_file, "%s", "write_hit ");
    fprintf(stats_file, "%d", (int)(core->num_write_hits));
    fprintf(stats_file, "%s", "\n");

    fprintf(stats_file, "%s", "read_miss ");
    fprintf(stats_file, "%d", (int)(core->num_read_miss));
    fprintf(stats_file, "%s", "\n");

    fprintf(stats_file, "%s", "write_miss ");
    fprintf(stats_file, "%d", (int)(core->num_write_miss));
    fprintf(stats_file, "%s", "\n");

    fprintf(stats_file, "%s", "decode_stall ");
    fprintf(stats_file, "%d", (int)(core->num_of_decode_stage_stalls));
    fprintf(stats_file, "%s", "\n");

    fprintf(stats_file, "%s", "mem_stall ");
    fprintf(stats_file, "%d", (int)(core->num_of_memory_stage_stalls));
}