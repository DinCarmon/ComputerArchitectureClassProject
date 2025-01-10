#pragma once

#include <limits.h>
#include <stdbool.h>

#define NUM_REQUESTORS 4  // Fixed number of BusRequestors

// Define bus status constants in uppercase
#define BUS_FREE   1    // The bus is free and available for use
#define BUS_RD     2    // The bus is performing a read operation
#define BUS_RDX    3    // The bus is performing a read-exclusive operation
#define BUS_INVALID 4   // The bus is in an invalid state (e.g., error state or not in use)
#define BUS_FLUSH 5		// the bus is performing flush
#define BUS_BEFORE_FLUSH 6 // the bus is goint to flush


// Constants for cache configuration
#define DSRAM_SIZE         256  // Number of cache lines in DSRAM (256 rows)
#define TSRAM_SIZE         64   // Number of cache lines in TSRAM (64 rows)
#define WORD_SIZE          4    // Size of a word in bytes (1 word = 4 bytes)
#define TAG_SIZE           12   // Tag size in bits
#define INDEX_SIZE         6    // Index size in bits (64 rows in TSRAM)
#define BLOCK_OFFSET_SIZE  2    // Block offset size in bits (4 bytes per word)
#define STATE_SIZE         2    // State size in bits (Modified, Exclusive, Shared, Invalid)
#define BLOCK_SIZE		   4	// size of memory block

// Define the cache states using #define
#define INVALID            0
#define SHARED             1
#define EXCLUSIVE          2
#define MODIFIED           3