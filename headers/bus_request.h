#pragma once

// Define the BusRequest struct
typedef struct {
    int operation;   // Operation type (e.g., read, write, invalidate)
    int address;     // Address associated with the bus request
    int start_cycle; // Cycle when the request started
} BusRequest;

// Function to create and initialize a BusRequest
BusRequest bus_request_create(void);
