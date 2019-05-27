//
// Created by Manuel on 2019-05-25.
//

#ifndef SIMULATION_DEFINITIONS_H
#define SIMULATION_DEFINITIONS_H

#include "flight.h"
#include "airport.h"

#define FLIGHT_MAX_COUNT    1000
#define AIRPORT_MAX_COUNT   100

// X X X X X:X X X = 15
#define FLIGHT_DATA_MIN_SIZE    15
#define FLIGHT_DATA_MAX_SIZE    100

#define MEM_EQUAL   0

typedef enum {
    READ_FLIGHT_INFO = 0u,
    SIMULATE,
    SIMULATION_COMPLETE
} simulation_states_t;

typedef struct {
    flight_t            flights[FLIGHT_MAX_COUNT];
    airport_t           airports[AIRPORT_MAX_COUNT];
    uint16_t            flight_count;
    uint16_t            airport_count;
    uint32_t            clock;
    simulation_states_t state;
    bool                complete;
} simulation_param_t;

#endif //SIMULATION_DEFINITIONS_H
