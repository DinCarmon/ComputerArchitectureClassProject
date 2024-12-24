#include "cache.h"
#include <string.h>

// Function to create and initialize a Cache instance
Cache cache_create(void) {
    Cache new_cache = { 0 };  // Initialize all fields to zero
    return new_cache;
}

// Function to extract the tag from an address  
int get_tag(unsigned int address) {
    // Shift the address right by the number of bits for index and block offset,
    // then mask to ensure only the tag bits are kept (12 bits for tag).
    return (address >> (INDEX_SIZE + BLOCK_OFFSET_SIZE)) & ((1 << TAG_SIZE) - 1);
}

// Function to extract the index from an address
int get_index(unsigned int address) {
    // Extract index by shifting the address right by the number of block offset bits,
    // then apply a mask to extract the index bits.
    return (address >> BLOCK_OFFSET_SIZE) & ((1 << INDEX_SIZE) - 1);  // Mask the index bits
}

// Function to check if an address is in the cache and return valid bits if the tag is found
int in_cache(unsigned int address, Cache* cache) {
    int index = get_index(address);  // Get the index from the address
    int tag = get_tag(address);  // Get the tag from the address

    // Check if the tag matches and if the state is not Invalid
    if (cache->tsram[index].tag == tag && cache->tsram[index].state != INVALID) {
        return 1;  // Cache hit
    }
    return 0;  // Cache miss
}

// Function to read data from the cache (entire row, not just byte-wise)
unsigned int read_cache(unsigned int address, Cache* cache) {
    // First check if the address is in the cache
    if (in_cache(address, cache)) {
        int index = get_index(address);  // Get the index from the address
        int block_offset = address & ((1 << BLOCK_OFFSET_SIZE) - 1);  // Get the block offset

        // Calculate the address in DSRAM by using index * WORD_SIZE + block_offset
        int dsm_address = index * WORD_SIZE + block_offset;

        // Access the correct word in DSRAM
        unsigned int word_data = cache->dsram[dsm_address];

        return word_data;  // Return the appropriate word from the cache line
    }
    return 0;  // Cache miss, return 0 or handle the miss case as needed
}

// Function to write data to the cache (entire row, not just byte-wise)
int write_cache(unsigned int address, Cache* cache, unsigned int data) {
    // First check if the address is in the cache
    if (in_cache(address, cache)) {
        int index = get_index(address);  // Get the index from the address
        int block_offset = address & ((1 << BLOCK_OFFSET_SIZE) - 1);  // Get the block offset

        // Calculate the address in DSRAM by using index * WORD_SIZE + block_offset
        int dsm_address = index * WORD_SIZE + block_offset;

        // Write the modified row back to DSRAM
        cache->dsram[dsm_address] = data;

        // Set the cache line state to Modified
        cache->tsram[index].state = MODIFIED;

        return 1;  // Write success
    }
    return 0;  // Cache miss, return failure
}
