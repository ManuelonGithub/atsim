//
// Created by Manuel on 2019-05-21.
//

#ifndef ASSIGNMENT_1_FLIGHT_H
#define ASSIGNMENT_1_FLIGHT_H

#include <stdio.h>
#include <stdbool.h>
#include "airport.h"

#define ID_LENGTH   5

#define TAXI_DURATION   10u

typedef enum {
    STAND_BY,
    DEPARTURE_TAXI,
    WAIT_TO_TAKEOFF,
    EN_ROUTE,
    WAIT_TO_LAND,
    ARRIVAL_TAXI,
    COMPLETE
} flight_states_t;

typedef struct {
    uint16_t scheduled;
    uint16_t flight;
    uint16_t departure;
    uint16_t arrival;
} flight_times_t;

typedef struct {
    char            carrier[ID_LENGTH];
    uint16_t        number;
    uint16_t        ID;
    airport_t *     origin;
    airport_t *     destination;
    flight_times_t  time;
    flight_states_t state;
} flight_t;

bool UpdateFlight (flight_t *flight, uint16_t timer);

#endif //ASSIGNMENT_1_FLIGHT_H
