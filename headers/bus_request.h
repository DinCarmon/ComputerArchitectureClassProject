#ifndef BUS_REQUEST_H
#define BUS_REQUEST_H

#include <stdint.h>

// Define the BusRequest struct
typedef struct busRequest{
    uint32_t operation;   // Operation type (e.g., read, write, invalidate)
    uint32_t address;     // Address associated with the bus request
} BusRequest;

// Function to create and initialize a BusRequest
BusRequest createBusRequestStruct(void);

#endif