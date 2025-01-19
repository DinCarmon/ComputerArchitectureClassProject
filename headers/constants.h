#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <math.h>
#include <stdint.h>

#define  LOG2                                 (log(2))
const int32_t BYTE_SIZE_IN_BITS                 = 8;

// Registers constants
const int32_t NUM_REGISTERS_PER_CORE            = 16;           // Total number of registers
const int32_t REGISTER_SIZE_IN_BITS             = 32;           // Size of each register in bits
const int32_t IMMEDIATE_REGISTER_INDEX          = 1;            // Special register for immediate values
const int32_t ZERO_REGISTER_INDEX               = 0;            // Register that always contains zero
const int32_t INSTRUCTION_MEMORY_DEPTH          = 1024;         // Each core has its own instruction memory
const int32_t PC_REGISTER_SIZE_IN_BITS          = 10;           // (log(INSTRUCTION_MEMORY_DEPTH) / LOG2)

// Cmd constants
const int32_t CMD_SIZE_IN_BITS                  = 32;
const int32_t IMMEDIATE_FIELD_SIZE_IN_BITS      = 12;
const int32_t REGISTER_INDEX_FIELD_SIZE_IN_BITS = 4;            // (log(NUM_REGISTERS_PER_CORE) / LOG2)
const int32_t OPCODE_FIELD_SIZE_IN_BITS         = 8;


const int32_t NUM_OF_CORES                      = 4;            // Num of cores in the architecture

// Main memory constants
const int32_t WORD_SIZE_IN_BITS                 = 32;
const int32_t MEMORY_DEPTH                      = 1048576;      // (pow(2, 20))

// Bus constants
const int32_t BUS_ORIGIN_ID_LINE_SIZE_IN_BITS   = 3;
const int32_t BUS_CMD_LINE_SIZE_IN_BITS         = 2;
const int32_t BUS_ADDRESS_LINE_SIZE_IN_BITS     = 20;           // (log(MEMORY_DEPTH) / LOG2)
const int32_t BUS_DATA_LINE_SIZE_IN_BITS        = WORD_SIZE_IN_BITS;
const int32_t BUS_SHARED_LINE_SIZE_IN_BITS      = 1;


// Block constants
const int32_t DATA_CACHE_WORD_DEPTH             = 256;          // How many words in each data cache.
const int32_t DATA_CACHE_BLOCK_DEPTH            = 4;            // How many words in each block.
const int32_t OFFSET_BIT_LENGTH                 = 2;            // ((log(DATA_CACHE_BLOCK_DEPTH) / LOG2))
const int32_t DATA_CACHE_NUM_OF_BLOCKS_IN_CACHE = 64;           // Number of cache lines in TSRAM (64 rows)
const int32_t INDEX_BIT_LENGTH                  = 6;            // ((log(DATA_CACHE_NUM_OF_BLOCKS_IN_CACHE) / LOG2))
const int32_t TAG_FIELD_SIZE_IN_BITS            = 12;           // ((log(MEMORY_DEPTH) / LOG2) - (log(DATA_CACHE_DEPTH) / LOG2))
const int32_t STATUS_BITS_SIZE_FOR_BLOCK        = 2;

// Default instruction memory file prefix
const char DEFAULT_INSTRUCTION_MEMORY_FILE_PATH_PREFIX[]    = "imem";
// Default main memory file path
const char DEFAULT_MAIN_MEMORY_FILE_INPUT_PATH[]            = "memin.txt";
const char DEFAULT_MAIN_MEMORY_FILE_OUTPUT_PATH[]           = "memout.txt";
// Default register file prefix
const char DEFAULT_REGISTER_FILE_PATH_PREFIX[]              = "regout";
// Default trace paths
const char DEFAULT_CORE_TRACE_FILE_PATH_PREFIX[]            = "core";
const char DEFAULT_CORE_TRACE_FILE_PATH_SUFFIX[]            = "trace.txt";
const char DEFAULT_BUS_TRACE_PATH[]                         = "bustrace.txt";
// Default caches paths
const char DEFAULT_DSRAM_CACHE_FILE_PATH_PREFIX[]           = "dsram";
const char DEFAULT_TSRAM_CACHE_FILE_PATH_PREFIX[]           = "tsram";
// Default stat paths
const char DEFAULT_STATS_FILE_PATH_PREFIX[]                 = "stats";

const ssize_t MAX_ERROR_MESSAGE_SIZE                                = 1000;
const ssize_t MAX_PATH_SIZE                                         = 1000;

// Define the CacheLine structure for DSRAM (1 word per line)
typedef uint32_t CacheLine;  // Each cache line contains a 32-bit word (int)

/**
 * OpcodeType - Enum representing all possible CPU instructions.
 * The enum value represents its opcode number
 */
typedef enum opcodeType {
    Add     = 0,    // R[rd] = R[rs] + R[rt]
    Sub     = 1,    // R[rd] = R[rs] - R[rt]
    And     = 2,    // R[rd] = R[rs] & R[rt]
    Or      = 3,    // R[rd] = R[rs] | R[rt]
    Xor     = 4,    // R[rd] = R[rs] ^ R[rt]
    Mul     = 5,    // R[rd] = R[rs] * R[rt]
    Sll     = 6,    // R[rd] = R[rs] << R[rt]
    Sra     = 7,    // R[rd] = R[rs] >> R[rt] (arithmetic)
    Srl     = 8,    // R[rd] = R[rs] >> R[rt] (logical)
    Beq     = 9,    // if (R[rs] == R[rt]) pc = R[rd][9:0]
    Bne     = 10,   // if (R[rs] != R[rt]) pc = R[rd][9:0]
    Blt     = 11,   // if (R[rs] < R[rt]) pc = R[rd][9:0]
    Bgt     = 12,   // if (R[rs] > R[rt]) pc = R[rd][9:0]
    Ble     = 13,   // if (R[rs] <= R[rt]) pc = R[rd][9:0]
    Bge     = 14,   // if (R[rs] >= R[rt]) pc = R[rd][9:0]
    Jal     = 15,   // R[15] = next inst addr, pc = R[rd][9:0]
    Lw      = 16,   // R[rd] = MEM[R[rs]+ R[rt]]
    Sw      = 17,   // MEM[R[rs]+R[rt]] = R[rd]
    Halt    = 20    // Halt this core
} OpcodeType;

/**
 * Cache block status for MESI protocol.
 * The enum value represents its value as stated in a line in the TSRAM memory
 */
typedef enum cacheLineStatus {
    Invalid         = 0,    // Cache block is invalid
    Shared          = 1,     // Cache block is shared
    Exclusive       = 2,  // Cache block is exclusive
    Modified        = 3    // Cache block is modified
} CacheLineStatus;

// Define the TSRAM memory - The data structure holding metadata on block in cache
// structure: tag and state
typedef struct tagState {
    uint32_t            tag;       // Tag bits (12 bits)
    CacheLineStatus     state;   // State bits (2 bits)
} TagState;

/**
 * BusOriginId - Identifies different originator of transaction on the bus
 * The enum value represents the value on the bus
 */
//typedef enum busOriginId {
//    Core0      = 0,
//    Core1      = 1,
//    Core2      = 2,
//    Core3      = 3,
//    MainMemory = 4
//} BusOriginId;

/**
 * Bus operation types for MESI protocol.
 * The enum value represents the value on the bus_cmd line.
 */
typedef enum busCmd {
    NoCmd   = 0,
    BusRd   = 1,        // Bus read operation
    BusRdX  = 2,        // Bus read exclusive operation
    Flush   = 3         // Cache flush operation
} BusCmd;

/**
 * Bus encoding for whether the current transaction 
 * is of a block which is in another core cache
 */
typedef enum busBlockSharingStatus {
    BlockIsNotShared    = 0,    // Default value for the bus_shared line
    BlockIsShared       = 1
} BusBlockSharingStatus;

#endif
