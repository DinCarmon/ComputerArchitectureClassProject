#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <math.h>
#include <stdint.h>

#define LOG2                                 (log(2))
#define NUM_OF_CORES                         4            // Num of cores in the architecture
#define BYTE_SIZE_IN_BITS                    8

// Registers constants
#define NUM_REGISTERS_PER_CORE               16           // Total number of registers
#define REGISTER_SIZE_IN_BITS                32           // Size of each register in bits
#define IMMEDIATE_REGISTER_INDEX             1            // Special register for immediate values
#define ZERO_REGISTER_INDEX                  0            // Register that always contains zero
#define INSTRUCTION_MEMORY_DEPTH             1024         // Each core has its own instruction memory
#define PC_REGISTER_SIZE_IN_BITS             10           // (log(INSTRUCTION_MEMORY_DEPTH) / LOG2)

// Cmd constants
#define CMD_SIZE_IN_BITS                     32
#define IMMEDIATE_FIELD_SIZE_IN_BITS         12
#define REGISTER_INDEX_FIELD_SIZE_IN_BITS    4            // (log(NUM_REGISTERS_PER_CORE) / LOG2)
#define OPCODE_FIELD_SIZE_IN_BITS            8

// Main memory constants
#define WORD_SIZE_IN_BITS                    32
#define WORD_SIZE                            (WORD_SIZE_IN_BITS / BYTE_SIZE_IN_BITS)    // Size of a word in bytes (1 word = 4 bytes)
#define MEMORY_DEPTH                         1048576      // (pow(2, 20))

// Bus constants
#define BUS_ORIGIN_ID_LINE_SIZE_IN_BITS      3
#define BUS_CMD_LINE_SIZE_IN_BITS            2
#define BUS_ADDRESS_LINE_SIZE_IN_BITS        20           // (log(MEMORY_DEPTH) / LOG2)
#define BUS_DATA_LINE_SIZE_IN_BITS           WORD_SIZE_IN_BITS
#define BUS_SHARED_LINE_SIZE_IN_BITS         1

// Block constants
#define DATA_CACHE_WORD_DEPTH                256          // How many words in each data cache.. This is the size of the dsram in words
#define DATA_CACHE_BLOCK_DEPTH               4            // How many words in each block.
#define OFFSET_BIT_LENGTH                    2            
#define DATA_CACHE_NUM_OF_BLOCKS_IN_CACHE    64           // Number of cache lines in TSRAM (64 rows)
#define TSRAM_SIZE                           DATA_CACHE_NUM_OF_BLOCKS_IN_CACHE   // Number of cache lines in TSRAM - as the number of blocks the cache
#define INDEX_BIT_LENGTH                     6            // ((log(DATA_CACHE_NUM_OF_BLOCKS_IN_CACHE) / LOG2)) - index size in bits
#define TAG_FIELD_SIZE_IN_BITS               12           // ((log(MEMORY_DEPTH) / LOG2) - (log(DATA_CACHE_DEPTH) / LOG2))
#define STATE_SIZE                           2            // State size in bits (Modified, Exclusive, Shared, Invalid)
#define BLOCK_OFFSET_SIZE                    2            // Block offset size in bits (4 bytes per word). // ((log(DATA_CACHE_BLOCK_DEPTH) / LOG2))

#define NUM_OF_STAGES_PER_CORE               5

#define MAIN_MEMORY_ANSWERING_DELAY          16

// Default instruction memory file prefix
#define DEFAULT_INSTRUCTION_MEMORY_FILE_PATH_PREFIX    "imem"
// Default main memory file path
#define DEFAULT_MAIN_MEMORY_FILE_INPUT_PATH            "memin.txt"
#define DEFAULT_MAIN_MEMORY_FILE_OUTPUT_PATH           "memout.txt"
// Default register file prefix
#define DEFAULT_REGISTER_FILE_PATH_PREFIX              "regout"
// Default trace paths
#define DEFAULT_CORE_TRACE_FILE_PATH_PREFIX            "core"
#define DEFAULT_CORE_TRACE_FILE_PATH_SUFFIX            "trace.txt"
#define DEFAULT_BUS_TRACE_PATH                         "bustrace.txt"
// Default caches paths
#define DEFAULT_DSRAM_CACHE_FILE_PATH_PREFIX           "dsram"
#define DEFAULT_TSRAM_CACHE_FILE_PATH_PREFIX           "tsram"
// Default stat paths
#define DEFAULT_STATS_FILE_PATH_PREFIX                 "stats"

#define MAX_ERROR_MESSAGE_SIZE                         1000
#define MAX_PATH_SIZE                                  1000

// Define the CacheLine structure for DSRAM (1 word per line)
typedef uint32_t CacheLine;  // Each cache line contains a 32-bit word (uint32_t)

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
    INVALID         = 0,    // Cache block is invalid
    SHARED          = 1,     // Cache block is shared
    EXCLUSIVE       = 2,  // Cache block is exclusive
    MODIFIED        = 3    // Cache block is modified
} CacheLineStatus;

/**
 * BusOriginId - Identifies different originator of transaction on the bus
 * The enum value represents the value on the bus
 */
typedef enum busOriginId
{
    CORE_0_BUS_ORIGIN      = 0,
    CORE_1_BUS_ORIGIN      = 1,
    CORE_2_BUS_ORIGIN      = 2,
    CORE_3_BUS_ORIGIN      = 3,
    MAIN_MEMORY_BUS_ORIGIN = 4
} BusOriginId;

/**
 * Bus operation types for MESI protocol.
 * The enum value represents the value on the bus_cmd line.
 */
typedef enum busCmd {
    NO_CMD   = 0,
    BUS_RD_CMD   = 1,           // Bus read operation
    BUS_RDX_CMD  = 2,           // Bus read exclusive operation
    FLUSH_CMD   = 3             // Cache flush operation
} BusCmd;

/**
 * Bus encoding for whether the current transaction 
 * is of a block which is in another core cache
 */
typedef enum busShareStatus {
    BLOCK_NOT_SHARED    = 0,    // Default value for the bus_shared line
    BLOCK_SHARED       = 1
} BusShareStatus;

/**
 * Bus state machine states
 */
typedef enum busStatus {
    BUS_FREE                = 1,    // The bus is free and available for use
    BUS_RD                  = 2,    // The bus is performing a read operation
    BUS_RDX                 = 3,    // The bus is performing a read-exclusive operation
    BUS_INVALID             = 4,    // The bus is in an invalid state (e.g., error state or not in use)
    BUS_FLUSH               = 5,	// the bus is performing flush
    BUS_BEFORE_FLUSH        = 6,    // the bus is goint to flush
    BUS_CACHE_INTERRUPTED   = 7,    // The state when a cache in mode m interupted and sent data
    BUS_WRITE               = 8     // the bus is writing data
} BusStatus;


#endif
