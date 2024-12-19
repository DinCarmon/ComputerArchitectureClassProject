#include "bus_request.h"

// Function to create and initialize a BusRequest with default values
BusRequest bus_request_create(void) {
    BusRequest request;

    // Initialize all fields to 0
    request.operation = 0;
    request.address = 0;
    request.start_cycle = 0;

    return request;
}
