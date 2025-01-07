#include "bus_snooper.h"

// Function to create and initialize a BusSnooper
BusSnooper bus_snooper_create(Core* my_core, BusManager* manager, int id) {
    BusSnooper snooper;
    snooper.my_core = my_core;
    snooper.manager = manager;
    snooper.busSnooperActive_now = false;
    snooper.busSnooperActive_updated = false;
    snooper.addr_to_flush = 0;
    snooper.start_flush_cycle = 0;
    snooper.id = id;
    return snooper;
}

// Function for the BusSnooper to snoop and react to bus commands
void snoop(BusSnooper* snooper, Cache* cache) {
    if (!snooper || !cache || !snooper->manager) {
        return; // Ensure all pointers are valid
    }

    BusManager* manager = snooper->manager;

    // Check if the bus command is BUS_RD or BUS_RDX and the originating ID isn't the snooper's ID
    if ((manager->bus_cmd == BUS_RD || manager->bus_cmd == BUS_RDX) && manager->bus_origid != snooper->id) {
        unsigned int address = manager->bus_addr;
        int state = getState(address, cache);

        // If the cache line is MODIFIED and the command is BUS_RD, return without doing anything
        if (state == MODIFIED && manager->bus_cmd == BUS_RD) {
            return;
        }

        // If the command is BUS_RDX and the address is in cache, invalidate the cache line
        if (manager->bus_cmd == BUS_RDX && inCache(address, cache)) {
            updateState(address, cache, INVALID);
        }

        // If the command is BUS_RD and the cache state is EXCLUSIVE, update to SHARED
        if (manager->bus_cmd == BUS_RD && state == EXCLUSIVE) {
            updateState(address, cache, SHARED);
        }
    }
}
