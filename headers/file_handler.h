#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <stdio.h>    // for FILE
#include <stdbool.h>  // for bool
#include <stdint.h>   // for uint32_t

#include "constants.h"
#include "main_memory.h"

struct core;

/**
 * Opens a file with the given path and mode.
 *
 * @param filePath  Path to the file.
 * @param mode      Single-character mode ('r', 'w', etc.).
 * @return          FILE pointer on success; NULL on error.
 */
FILE* openFile(char* filePath, char mode);

/**
 * Checks whether a given string (up to length 8) contains only hexadecimal characters.
 *
 * @param string    The string to test.
 * @return          true if the string is valid hex, false otherwise.
 */
bool isHexString(char* string);

/**
 * Converts an 8-character hex string to a 32-bit unsigned integer.
 *
 * @param string    The hex string (should be at least 8 chars, ignoring any extra).
 * @return          The 32-bit unsigned integer value of the hex string.
 */
uint32_t hexStringToUint32(char* string);

/**
 * Reads one hexadecimal number from a file (as 8 hex chars) and stores it in `num`.
 *
 * @param file      The file to read from.
 * @param num       Pointer to a uint32_t where the read value is stored.
 * @return          true if a number was read, false if end-of-file was reached.
 */
bool readNumFromFileInHex(FILE* file, uint32_t* num);

/**
 * Converts a 32-bit unsigned integer into a hex string of length `hexRepresentationLen`.
 *
 * @param num                   The number to convert.
 * @param str                   The destination buffer for the hex string.
 * @param hexRepresentationLen  The length of the hex portion to store (e.g., 8 for 8 hex digits).
 */
void numberToHexString(uint32_t num, char* str, uint32_t hexRepresentationLen);

/**
 * Writes a 32-bit unsigned integer (in 8-hex-digit form) plus newline to a file.
 *
 * @param file  The file to write to.
 * @param num   The value to write (as hex).
 */
void writeNumberToFileInHex(FILE* file, uint32_t num);

/**
 * Writes a 3-hex-digit address to a file (e.g., instruction address).
 *
 * @param file               The file to write to.
 * @param instructionAddress The address to write (in 3-hex-digit form).
 */
void writeInstructionAddressToFile(FILE* file, uint32_t instructionAddress);

/**
 * Opens various files (instruction memories, main memory I/O, trace files, etc.)
 * either by default paths (if argc == 2) or from command-line arguments (if argc == 28).
 *
 * @param argc                    Argument count from main().
 * @param argv                    Argument vector from main().
 * @param instructionMemoryFiles  Array to store FILE pointers for instruction-memory inputs.
 * @param mainMemoryInputFile     Pointer to a FILE pointer for main memory input.
 * @param mainMemoryOutputFile    Pointer to a FILE pointer for main memory output.
 * @param registerFiles           Array of FILE pointers for register outputs.
 * @param coreTraceFiles          Array of FILE pointers for core trace outputs.
 * @param busTraceFile            Pointer to a FILE pointer for bus trace output.
 * @param dataCacheFiles          Array of FILE pointers for data cache outputs.
 * @param statusCacheFiles        Array of FILE pointers for status cache outputs.
 * @param statsFiles              Array of FILE pointers for stats outputs.
 */
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
    FILE* statsFiles[NUM_OF_CORES]);

/**
 * Loads instructions from a file into instructionMemory (which is an array of ints).
 *
 * @param instructionMemoryFile   File containing the instructions (in hex).
 * @param instructionMemory       Pointer to an integer array to store the instructions.
 */
void loadInstructionMemory(FILE* instructionMemoryFile, uint32_t* instructionMemory);

/**
 * Loads data from a file into the main memory structure.
 *
 * @param mainMemoryFile  File to read from (in hex).
 * @param mem             Pointer to a MainMemory structure.
 */
void loadMainMemory(FILE* mainMemoryFile, MainMemory* mem);

/**
 * Writes the contents of the main memory structure to a file (in hex).
 *
 * @param mainMemoryFile  File to write to.
 * @param mem             Pointer to a MainMemory structure.
 */
void writeMainMemory(FILE* mainMemoryFile, MainMemory* mem);

/**
 * Writes registers R2..R(END) in hex to a file.
 *
 * @param registerFile    File to write to.
 * @param registerArr     Array of register values.
 */
void writeRegisterFile(FILE* registerFile, uint32_t* registerArr);

/**
 * Writes a single line of core trace info (pipeline stage addresses, registers, etc.) to a file.
 *
 * @param coreTraceFile   File to write the trace line to.
 * @param core            Pointer to the Core whose state is being traced.
 * @param cycle           The current cycle number (for logging).
 */
void writeCoreTrace(FILE* coreTraceFile, Core* core, uint32_t cycle);

// load instructionmemory for all cores
void loadCoresImemory(struct core (*cores)[NUM_OF_CORES], FILE* instructionMemoryFiles[NUM_OF_CORES]);

#endif /* FILE_HANDLER_H */
