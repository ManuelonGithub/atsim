//
// Created by Manuel on 2019-05-25.
//

#ifndef ATSIM_DEFINITIONS_H
#define ATSIM_DEFINITIONS_H

#include "airport.h"

// Plane, Flight and airport count are easily scalable as the simulation
// requirements grows.
#define PLANE_MAX_COUNT     1000
#define FLIGHT_MAX_COUNT    1000
#define AIRPORT_MAX_COUNT   256

// Shortest valid flight input looks like this: (X being placeholder characters)
// X X X X X:X X X
// = 15 total characters (including whitespace)
#define FLIGHT_DATA_MIN_SIZE    15
#define FLIGHT_DATA_MAX_SIZE    100

#define SIMULATION_MAX_TIME 24*60

typedef enum {
    READ_FLIGHT_INFO = 0u,
    SIMULATE,
    SIMULATION_COMPLETE
} simulation_states_t;

typedef struct {
    plane_t             planes[PLANE_MAX_COUNT];
    flight_t            flights[FLIGHT_MAX_COUNT];
    airport_t           airports[AIRPORT_MAX_COUNT];
    pthread_t           airport_threads[AIRPORT_MAX_COUNT];
    pthread_barrier_t   airport_start_sync;
    pthread_barrier_t   airport_end_sync;
    uint16_t            flight_count;
    uint16_t            airport_count;
    uint32_t            clock;
    simulation_states_t state;
    bool                complete;
    bool                thread_done;
} simulation_param_t;

#endif //ATSIM_DEFINITIONS_H
