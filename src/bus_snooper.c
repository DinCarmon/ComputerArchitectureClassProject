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

// Function for the BusSnooper to snoop and react to bus commands
void snoop(BusSnooper* snooper, Cache* cache, BusManager* manager) {
    if (!snooper || !cache || !manager) {
        return; // Ensure all pointers are valid
    }

    // Check if the bus command is BUS_RD or BUS_RDX and the originating ID isn't the snooper's ID
    if ((manager->bus_cmd.now == BUS_RD || manager->bus_cmd.now == BUS_RDX) && manager->bus_origid.now != snooper->id) {

        uint32_t address = manager->bus_addr.now;
        int state = get_state(address, cache);
        if (in_cache(address, cache)) {

            // If the cache line is MODIFIED and the command is BUS_RD
            if (state == MODIFIED && manager->bus_cmd.now == BUS_RD) {
                manager->interuptor_id = snooper->id;
                manager->Interupted.updated = true;
                update_state(address, cache, Shared);
                return;
            }

            // If the cache line is modified and the command is bus rdx, interupt bus, and change state to invalid
            if (state == MODIFIED && manager->bus_cmd.now == BUS_RDX) {
                manager->interuptor_id = snooper->id;
                manager->Interupted.updated = true;
                update_state(address, cache, INVALID);
                return;
            }

            // If the command is BUS_RDX and the address is in cache, invalidate the cache line
            if (manager->bus_cmd.now == BUS_RDX) {
                update_state(address, cache, INVALID);
            }

            // If the command is BUS_RD and the cache state is EXCLUSIVE or SHARED, update to SHARED
            if (manager->bus_cmd.now == BUS_RD) {
                update_state(address, cache, SHARED);
                manager->bus_shared.updated = true;
            }
        }

    }
}


// Function to destroy a BusSnooper and free its memory
void bus_snooper_destroy(BusSnooper* snooper) {
    if (snooper) {
        free(snooper);
    }
}


