#include "bus_snooper.h"
#include <stdlib.h>
#include <stdio.h>


// Function to create and initialize a BusSnooper on the heap
BusSnooper* bus_snooper_create(int id) {
    BusSnooper* snooper = (BusSnooper*)malloc(sizeof(BusSnooper));
    if (!snooper) {
        fprintf(stderr, "Failed to allocate memory for BusSnooper.\n");
        exit(EXIT_FAILURE);
    }
    snooper->busSnooperActive.now = false;
    snooper->busSnooperActive.updated = false;
    snooper->addr_to_flush = 0;
    snooper->start_flush_cycle = 0;
    snooper->id = id;

    return snooper;
}

// Function to destroy a BusSnooper and free its memory
void bus_snooper_destroy(BusSnooper* snooper) {
    if (snooper) {
        free(snooper);
    }
}


