#pragma once
#include "defines.h"



// Define the CacheLine structure for DSRAM (1 word per line)
typedef uint32_t CacheLine;  // Each cache line contains a 32-bit word (uint32_t)

// Define the TSRAM structure for tag and state
typedef struct {
    uint32_t tag : TAG_SIZE;    // Tag bits (12 bits)
    uint32_t state : STATE_SIZE; // State bits (2 bits)
} TagState;

// Define the Cache structure
typedef struct {
    CacheLine dsram[DSRAM_SIZE];  // Data SRAM (256 rows of 1 word per row)
    TagState tsram[TSRAM_SIZE];   // Tag SRAM (64 rows, each 14 bits: 12 for tag + 2 for state)
} Cache;

// Function declarations

// Function to create and initialize a Cache instance
Cache cache_create(void);

// Function to extract the tag from an address  
int get_tag(uint32_t address);

// Function to extract the index from an address
int get_index(uint32_t address);

int get_block_offset(uint32_t address);

// Function to check if an address is in the cache and return valid bits if the tag is found
int in_cache(uint32_t address, Cache* cache);

// Function to read data from the cache
uint32_t read_cache(uint32_t address, Cache* cache);

// Function to write data to the cache
int write_cache(uint32_t address, Cache* cache, uint32_t data);

// Function to get state of address
int get_state(uint32_t address, Cache* cache);

// Function to update state in cache
void update_state(uint32_t address, Cache* cache, uint32_t state);