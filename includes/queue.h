/*
 * File: queue.h
 * Author: Manuel Burnay
 * Date: May 20, 2019
 * Purpose:
 *      This file contains the declarations and definitions of the
 *      structures, constants and functions used by the flight queue
 *      or used to manage the flight queue.
 *
 */

#ifndef ATSIM_QUEUE_H
#define ATSIM_QUEUE_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    STAND_BY,
    WAIT_FOR_PLANE,
    DEPARTURE_TAXI,
    WAIT_TO_TAKEOFF,
    EN_ROUTE,
    WAIT_TO_LAND,
    ARRIVAL_TAXI,
    GROOM_PLANE,
    COMPLETE
} flight_states_t;

typedef struct {
    uint16_t scheduled;
    uint16_t flight;
    uint16_t departure;
    uint16_t land;
    uint16_t arrival;
} flight_times_t;

#define CARRIER_ID_LENGTH     2
#define CARRIER_ID_STR_SIZE   CARRIER_ID_LENGTH+1

typedef struct Flight {
    char                carrier[CARRIER_ID_STR_SIZE];
    uint16_t            number;
    struct Plane*       plane;
    struct Airport*     origin;     // This solves circular dependency
    struct Airport*     destination;
    flight_times_t      time;
    flight_states_t     state;
} flight_t;


#define FLIGHT_QUEUE_SIZE   256u
#define FLIGHT_QUEUE_SIZE_MASK  FLIGHT_QUEUE_SIZE-1u

typedef struct FlightQueue {
    uint32_t    head;
    uint32_t    tail;
    flight_t*   buffer[FLIGHT_QUEUE_SIZE];
} flight_queue_t;


typedef enum QueueTypes {
    DEPARTURE,
    ARRIVAL,
    QUEUE_TYPES
} queue_types_t;

#define CODE_LENGTH     3
#define CODE_STR_SIZE   CODE_LENGTH+1 // account for the Null termination

typedef struct Airport {
    flight_queue_t  departures_queue;
    flight_queue_t  arrivals_queue;
    char            code[CODE_STR_SIZE];
    queue_types_t   last_queue_type;
} airport_t;

typedef struct Plane {
    airport_t*  airport;
    bool        busy;
    uint16_t    groom;
    uint16_t    id;
} plane_t;

void init_queue (flight_queue_t * queue);
void dequeue(flight_queue_t *queue);
void enqueue(flight_queue_t *queue, flight_t *flight);
flight_t* peek(flight_queue_t *queue);
uint32_t size (flight_queue_t * queue);

#endif //ATSIM_QUEUE_H
