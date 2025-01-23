#include <stdlib.h>
#include <stdio.h>
#include "bus_snooper.h"
#include "bus_manager.h"
#include "core.h"

// Function to create and initialize a BusSnooper on the heap
void configure_bus_snooper(BusSnooper* snooper, struct core* myCore)
{
    snooper->busSnooperActive.now = false;
    snooper->busSnooperActive.updated = false;
    snooper->addr_to_flush = 0;
    snooper->start_flush_cycle = 0;
    snooper->myCore = myCore;
}

// Function for the BusSnooper to snoop and react to bus commands
void do_snoop_operation(BusSnooper* snooper)
{
    // Check if the bus command is BUS_RD or BUS_RDX and the originating ID isn't the snooper's ID
    if ((snooper->myCore->bus_manager->bus_cmd.now == BUS_RD_CMD ||
        snooper->myCore->bus_manager->bus_cmd.now == BUS_RDX_CMD) &&
        snooper->myCore->bus_manager->bus_origid.now != snooper->myCore->id)
    {
        uint32_t address = snooper->myCore->bus_manager->bus_line_addr.now;
        int state = get_state(address, &(snooper->myCore->cache_now));
        if (in_cache(address, &(snooper->myCore->cache_now)))
        {
            // If the cache line is MODIFIED and the command is BUS_RD
            if (state == MODIFIED && snooper->myCore->bus_manager->bus_cmd.now == BUS_RD_CMD)
            {
                interrupt_bus(snooper->myCore->bus_manager, snooper->myCore->id);
                update_state(address,
                             &(snooper->myCore->cache_updated),
                             SHARED);
                snooper->myCore->bus_manager->bus_shared.updated = BLOCK_SHARED;
                return;
            }

            // If the cache line is modified and the command is bus rdx, interupt bus, and change state to invalid
            if (state == MODIFIED && snooper->myCore->bus_manager->bus_cmd.now == BUS_RDX_CMD)
            {
                interrupt_bus(snooper->myCore->bus_manager, snooper->myCore->id);
                update_state(address,
                             &(snooper->myCore->cache_updated),
                             INVALID);
                return;
            }

            // If the command is BUS_RDX and the address is in cache, invalidate the cache line
            if (snooper->myCore->bus_manager->bus_cmd.now == BUS_RDX_CMD)
            {
                update_state(address,
                             &(snooper->myCore->cache_updated),
                             INVALID);
                return;
            }

            // If the command is BUS_RD and the cache state is EXCLUSIVE or SHARED (modified was handled before),
            // update to SHARED, and update bus_shared line
            if (snooper->myCore->bus_manager->bus_cmd.now == BUS_RD_CMD)
            {
                update_state(address,
                             &(snooper->myCore->cache_updated),
                             SHARED);
                snooper->myCore->bus_manager->bus_shared.updated = BLOCK_SHARED;
                return;
            }
        }
    }
}

