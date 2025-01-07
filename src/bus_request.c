#include "bus_request.h"

// Function to create and initialize a BusRequest with default values
BusRequest createBusRequestStruct(void) {
    BusRequest request;

    // Initialize all fields to 0
    request.operation = 0;
    request.address = 0;

    return request;
}
