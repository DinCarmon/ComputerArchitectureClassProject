#ifndef CACHE_H
#define CACHE_H

#include <stdint.h>
#include <stdbool.h>
#include "constants.h"
#include "flip_flop.h"

// Define the Cache structure
typedef struct cache {
    FlipFlop_CacheLine      dsram[DATA_CACHE_WORD_DEPTH];               // Data SRAM (256 rows of 1 word per row)
    FlipFlop_TagState       tsram[DATA_CACHE_NUM_OF_BLOCKS_IN_CACHE];   // Tag SRAM (64 rows, each 14 bits: 12 for tag + 2 for state)
} Cache;

// Function declarations

// Function to create and initialize a Cache instance
Cache cacheCreate(void);

// Function to extract the tag from an address  
uint32_t getTag(uint32_t address);

// Function to extract the index from an address
uint32_t getIndex(uint32_t address);

// Function to check if an address is in the cache and return valid bits if the tag is found
bool inCache(uint32_t address, Cache* cache);

/**
 * Function to read data from the cache.
 * Caller first need to check the address is in the cache
 */
uint32_t readCache(uint32_t address, Cache* cache);

// Function to write data to the cache
bool writeCache(uint32_t address, Cache* cache,  uint32_t data);

// Function to get state of address
CacheLineStatus getState(uint32_t address, Cache* cache);

// Function to update state in cache
void updateState(uint32_t address, Cache* cache, uint32_t state);

uint32_t getFirstAddressInBlock(uint32_t index, Cache* cache);

#endif