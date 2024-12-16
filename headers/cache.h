#pragma once

// Define the Cache struct
typedef struct {
    int DSRAM[64][4]; // Data SRAM memory: 64 rows x 4 columns
    int TSRAM[64][1]; // Tag SRAM memory: 64 rows x 1 column
} Cache;

// Function to create and initialize a Cache instance
Cache cache_create(void);

// Function to extract the tag from an address (bits 10 to 19)
int get_tag(int address);

// Function to extract the index from an address (bits 2 to 9)
int get_index(int address);

// Function to check if an address is in the cache and return valid bits if the tag is found
int in_cache(int address, Cache* cache);
