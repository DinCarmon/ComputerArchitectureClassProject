#include <string.h>
#include "cache.h"

// Function to create and initialize a Cache instance
Cache cacheCreate(void) {
    Cache newCache = { 0 };  // Initialize all fields to zero
    return newCache;
}

// Function to extract the tag from an address  
uint32_t getTag(uint32_t address) {
    // Shift the address right by the number of bits for index and block offset,
    // then mask to ensure only the tag bits are kept (12 bits for tag).
    return (address >> (INDEX_BIT_LENGTH + OFFSET_BIT_LENGTH)) & ((1 << TAG_FIELD_SIZE_IN_BITS) - 1);
}

// Function to extract the index from an address
uint32_t getIndex(uint32_t address) {
    // Extract index by shifting the address right by the number of block offset bits,
    // then apply a mask to extract the index bits.
    return (address >> OFFSET_BIT_LENGTH) & ((1 << INDEX_BIT_LENGTH) - 1);  // Mask the index bits
}

// Function to check if an address is in the cache and return valid bits if the tag is found
bool inCache(uint32_t address, Cache* cache) {
    uint32_t index = getIndex(address);  // Get the index from the address
    uint32_t addrTag = getTag(address);  // Get the tag from the address

    // Check if the tag matches and if the state is not Invalid
    if (cache->tsram[index].now.tag == addrTag && cache->tsram[index].now.state != Invalid) {
        return true;  // Cache hit
    }
    return false;  // Cache miss
}

// Function to get state of address
CacheLineStatus getState(uint32_t address, Cache* cache) {
    uint32_t index = getIndex(address);         // Get the index from the address
    CacheLineStatus addrTag = getTag(address);  // Get the tag from the address

    if (cache->tsram[index].now.tag != addrTag)
        return Invalid;

    return cache->tsram[index].now.state;
}

// Function to read data from the cache (entire row, not just byte-wise)
uint32_t readCache(uint32_t address, Cache* cache) {
    // First check if the address is in the cache
    if (inCache(address, cache)) {
        uint32_t index = getIndex(address);  // Get the index from the address
        uint32_t blockOffset = address & ((1 << OFFSET_BIT_LENGTH) - 1);  // Get the block offset

        // Calculate the address in DSRAM by using index * WORD_SIZE + block_offset
        uint32_t dataAddressInCache = index * DATA_CACHE_BLOCK_DEPTH + blockOffset;

        // Access the correct word in DSRAM
         uint32_t wordData = cache->dsram[dataAddressInCache].now;

        return wordData;  // Return the appropriate word from the cache line
    }

    return 0;  // Runtime should not get here
}

// Function to write data to the cache (entire row, not just byte-wise)
void writeCache(uint32_t address, Cache* cache,  uint32_t data) {
    // First check if the address is in the cache
    if (inCache(address, cache)) {
        uint32_t index = getIndex(address);  // Get the index from the address
        uint32_t blockOffset = address & ((1 << OFFSET_BIT_LENGTH) - 1);  // Get the block offset
        CacheLineStatus tag = getTag(address);  // Get the tag from the address

        // Calculate the address in DSRAM by using index * WORD_SIZE + block_offset
        uint32_t dataAddressInCache = index * DATA_CACHE_BLOCK_DEPTH + blockOffset;

        // Write the modified row back to DSRAM
        cache->dsram[dataAddressInCache].updated = data;

        cache->tsram[index].updated.tag = tag;

        // Set the cache line state to Modified
        cache->tsram[index].updated.state = Modified;
    }
}

// Function to update state in cache
void updateState(uint32_t address, Cache* cache, uint32_t state) {
    // First check if the address is in the cache

    uint32_t index = getIndex(address);  // Get the index from the address
    CacheLineStatus tag = getTag(address);  // Get the tag from the address

    cache->tsram[index].updated.tag = tag;
    cache->tsram[index].updated.state = state;
}

uint32_t getFirstAddressInBlock(uint32_t index, Cache* cache)
{
    uint32_t tagForIndex = cache->tsram[index].now.tag;
    return (tagForIndex << (INDEX_BIT_LENGTH + OFFSET_BIT_LENGTH) +
            index * DATA_CACHE_BLOCK_DEPTH);
}
