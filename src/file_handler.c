#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "constants.h"
#include "core.h"
#include "main_memory.h"
#include "file_handler.h"

FILE* openFile(char* filePath, char mode)
{
    FILE *file = fopen(filePath, &mode);
    if (file == NULL)
    {
        char errorMessage[MAX_ERROR_MESSAGE_SIZE] = "Error opening file: ";
        strncat(errorMessage, filePath, sizeof(errorMessage) - 1);
        strncat(errorMessage, " in mode: ", sizeof(errorMessage) - 1);
        strncat(errorMessage, &mode, 1);
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
        if (digit > '0' && digit <= '9')
            digitInt = digit - '0';
        if (digit > 'A' && digit <= 'F')
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
        return false;

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
                           FILE* instructionMemoryFiles[NUM_OF_CORES],
                           FILE** mainMemoryInputFile,
                           FILE** mainMemoryOutputFile,
                           FILE* registerFiles[NUM_OF_CORES],
                           FILE* coreTraceFiles[NUM_OF_CORES],
                           FILE** busTraceFile,
                           FILE* dataCacheFiles[NUM_OF_CORES],
                           FILE* statusCacheFiles[NUM_OF_CORES],
                           FILE* statsFiles[NUM_OF_CORES])
{
    if (argc == 2)
    {
        *mainMemoryInputFile = openFile(DEFAULT_MAIN_MEMORY_FILE_INPUT_PATH, 'r');
        *mainMemoryOutputFile = openFile(DEFAULT_MAIN_MEMORY_FILE_OUTPUT_PATH, 'w');
        *busTraceFile = openFile(DEFAULT_BUS_TRACE_PATH, 'w');
        for (int i = 0; i < NUM_OF_CORES; i++)
        {
            char coreNumStr[MAX_PATH_SIZE] = "";


            
            snprintf(coreNumStr, sizeof(coreNumStr), "%d", i);

            char instructionMemoryFilePath[MAX_PATH_SIZE] = "";
            strncat(instructionMemoryFilePath, DEFAULT_INSTRUCTION_MEMORY_FILE_PATH_PREFIX, sizeof(instructionMemoryFilePath) - 1);
            strncat(instructionMemoryFilePath, coreNumStr, sizeof(instructionMemoryFilePath) - 1);
            strncat(instructionMemoryFilePath, ".txt", sizeof(instructionMemoryFilePath) - 1);
            instructionMemoryFiles[i] = openFile(instructionMemoryFilePath, 'r');

            char registerFilePath[MAX_PATH_SIZE] = "";
            strncat(registerFilePath, DEFAULT_REGISTER_FILE_PATH_PREFIX, sizeof(registerFilePath) - 1);
            strncat(registerFilePath, coreNumStr, sizeof(registerFilePath) - 1);
            strncat(registerFilePath, ".txt", sizeof(registerFilePath) - 1);
            registerFiles[i] = openFile(registerFilePath, 'w');

            char coreTraceFilePath[MAX_PATH_SIZE] = "";
            strncat(coreTraceFilePath, DEFAULT_CORE_TRACE_FILE_PATH_PREFIX, sizeof(coreTraceFilePath) - 1);
            strncat(coreTraceFilePath, coreNumStr, sizeof(coreTraceFilePath) - 1);
            strncat(coreTraceFilePath, DEFAULT_CORE_TRACE_FILE_PATH_SUFFIX, sizeof(coreTraceFilePath) - 1);
            coreTraceFiles[i] = openFile(coreTraceFilePath, 'w');

            char dataCacheFilePath[MAX_PATH_SIZE] = "";
            strncat(dataCacheFilePath, DEFAULT_DSRAM_CACHE_FILE_PATH_PREFIX, sizeof(dataCacheFilePath) - 1);
            strncat(dataCacheFilePath, coreNumStr, sizeof(dataCacheFilePath) - 1);
            strncat(dataCacheFilePath, ".txt", sizeof(dataCacheFilePath) - 1);
            dataCacheFiles[i] = openFile(dataCacheFilePath, 'w');

            char statusCacheFilePath[MAX_PATH_SIZE] = "";
            strncat(statusCacheFilePath, DEFAULT_TSRAM_CACHE_FILE_PATH_PREFIX, sizeof(statusCacheFilePath) - 1);
            strncat(statusCacheFilePath, coreNumStr, sizeof(statusCacheFilePath) - 1);
            strncat(statusCacheFilePath, ".txt", sizeof(statusCacheFilePath) - 1);
            statusCacheFiles[i] = openFile(statusCacheFilePath, 'w');

            char statsFilePath[MAX_PATH_SIZE] = "";
            strncat(statsFilePath, DEFAULT_TSRAM_CACHE_FILE_PATH_PREFIX, sizeof(statsFilePath) - 1);
            strncat(statsFilePath, coreNumStr, sizeof(statsFilePath) - 1);
            strncat(statsFilePath, ".txt", sizeof(statsFilePath) - 1);
            statsFiles[i] = openFile(statsFilePath, 'w');
        }
    }
    else if (argc == 28)
    {
        *mainMemoryInputFile = openFile(argv[5], 'r');
        *mainMemoryOutputFile = openFile(argv[6], 'w');
        *busTraceFile = openFile(argv[15], 'w');
        for (int i = 0; i < NUM_OF_CORES; i++)
        {
            instructionMemoryFiles[i] = openFile(argv[1 + i], 'r');
            registerFiles[i] = openFile(argv[7 + i], 'w');
            coreTraceFiles[i] = openFile(argv[11 + i], 'w');
            dataCacheFiles[i] = openFile(argv[16 + i], 'w');
            statusCacheFiles[i] = openFile(argv[20 + i], 'w');
            statsFiles[i] = openFile(argv[24 + i], 'w');
        }
    }
    else
    {
        perror("Wrong number of arguments. \
                Either provide all file paths as decribed in the project description,\
                or provide no arguments at all");
    }
}

void loadInstructionMemory(FILE* instructionMemoryFile, uint32_t* instructionMemory)
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

void loadCoresImemory(Core (*cores)[NUM_OF_CORES], FILE* instructionMemoryFiles[NUM_OF_CORES]) {
    for (int i = 0; i < NUM_OF_CORES; i++)
    {
        loadInstructionMemory(instructionMemoryFiles[i], cores[i]->InstructionMemory);
    }
}

void loadMainMemory(FILE* mainMemoryFile, MainMemory* mem)
{
    uint32_t address = 0;
    while (address < (sizeof(mem) / sizeof(uint32_t)))
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
    while (address < (sizeof(mem) / (sizeof(uint32_t))))
    {
        uint32_t data = (*mem).memory[address];

        writeNumberToFileInHex(mainMemoryFile, data);
        
        address++;
    }
}

void writeRegisterFile(FILE* registerFile, uint32_t* registerArr)
{
    // Skip R0, R1
    for(int i = 2; i < NUM_REGISTERS_PER_CORE; i++)
    {
        writeNumberToFileInHex(registerFile, registerArr[i]);
    }
}

void writeCoreTrace(FILE* coreTraceFile,Core* core, uint32_t cycle)
{
    char cycleStr[MAX_PATH_SIZE] = "";
    snprintf(cycleStr, sizeof(cycleStr), "%d", cycle);
    fprintf(coreTraceFile, "%s", cycleStr);
    fprintf(coreTraceFile, " ");

    char instructionAddressTmpString[4];
    instructionAddressTmpString[3] = '\0';
    numberToHexString(core->fetch_stage.state.inputState.instructionAddress, instructionAddressTmpString, 3);
    fprintf(coreTraceFile, "%s", instructionAddressTmpString);
    fprintf(coreTraceFile, " ");

    numberToHexString(core->decode_stage.state.inputState.instructionAddress, instructionAddressTmpString, 3);
    fprintf(coreTraceFile, "%s", instructionAddressTmpString);
    fprintf(coreTraceFile, " ");

    numberToHexString(core->execute_stage.state.inputState.instructionAddress, instructionAddressTmpString, 3);
    fprintf(coreTraceFile, "%s", instructionAddressTmpString);
    fprintf(coreTraceFile, " ");

    numberToHexString(core->memory_stage.state.inputState.instructionAddress, instructionAddressTmpString, 3);
    fprintf(coreTraceFile, "%s", instructionAddressTmpString);
    fprintf(coreTraceFile, " ");

    numberToHexString(core->writeback_stage.state.inputState.instructionAddress, instructionAddressTmpString, 3);
    fprintf(coreTraceFile, "%s", instructionAddressTmpString);

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