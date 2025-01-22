#include "bus_snooper.h"
#include <stdlib.h>
#include <stdio.h>


// Function to create and initialize a BusSnooper on the heap
BusSnooper bus_snooper_create(int id) {
    BusSnooper snooper;

    snooper.busSnooperActive.now = false;
    snooper.busSnooperActive.updated = false;
    snooper.addr_to_flush = 0;
    snooper.start_flush_cycle = 0;
    snooper.id = id;

    return snooper;
}

