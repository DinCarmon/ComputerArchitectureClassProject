#include "cache.h"

// Function to create and initialize a Cache instance
Cache cache_create(void) {
    Cache newCache;

    // Zero out DSRAM and TSRAM
    memset(newCache.DSRAM, 0, sizeof(newCache.DSRAM));
    memset(newCache.TSRAM, 0, sizeof(newCache.TSRAM));

    return newCache;
}

// Function to extract the tag (bits 10 to 19) from an address
int get_tag(int address) {
    return (address >> 10) & 0x3FF; // Mask for 10 bits
}

// Function to extract the index (bits 2 to 9) from an address
int get_index(int address) {
    return (address >> 2) & 0xFF; // Mask for 8 bits (binary 11111111)
}

// Function to check if an address is in the cache and return valid bits if the tag is found
int in_cache(int address, Cache* cache) {
    // Extract the tag and index from the address
    int tag = get_tag(address);
    int index = get_index(address);

    // Extract the relevant bits of TSRAM[index]
    int tag_in_cache = cache->TSRAM[index][0] & 0xFFF; // Mask to get the lower 12 bits (bits 0-11)

    // Extract bits 12-13 from the address
    int valid_bits = (address >> 12) & 0x3; // Extract bits 12 and 13

    // If the tag matches, return the valid bits; otherwise, return 0
    return (tag == tag_in_cache) ? valid_bits : 0;
}
