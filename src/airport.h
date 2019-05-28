//
// Created by Manuel on 2019-05-21.
//

#ifndef AIRPORT_H
#define AIRPORT_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

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
    uint16_t land;
    uint16_t arrival;
} flight_times_t;

#define ID_LENGTH   5

typedef struct Flight {
    char                carrier[ID_LENGTH];
    uint16_t            number;
    uint16_t            ID;
    struct Airport *    origin;
    struct Airport *    destination;
    flight_times_t      time;
    flight_states_t     state;
} flight_t;

#define FLIGHT_QUEUE_SIZE   256u
#define FLIGHT_QUEUE_SIZE_MASK  FLIGHT_QUEUE_SIZE-1u

typedef struct {
    uint32_t    read_ptr;
    uint32_t    write_ptr;
    flight_t *  buffer[FLIGHT_QUEUE_SIZE];
} flight_queue_t;

typedef enum {
    DEPARTURE,
    ARRIVAL,
    QUEUE_TYPES
} queue_types_t;

#define EMPTY_QUEUE 0u
#define QUEUE_TYPE_MASK 1u
#define NEXT_QUEUE_TYPE(queue) ((queue + 1u) & QUEUE_TYPE_MASK)

#define CODE_LENGTH     3
#define CODE_STR_SIZE   CODE_LENGTH+1

typedef struct Airport {
    flight_queue_t  departures_queue;
    flight_queue_t  arrivals_queue;
    flight_queue_t  runway_queue;
    char            code[CODE_STR_SIZE];
    queue_types_t   last_queue_type;
} airport_t;

typedef struct {
    uint8_t hour;
    uint8_t minute;
} atsim_time_t;

#define TAXI_DURATION   10u

void init_queue (flight_queue_t * queue);
void pop (flight_queue_t * queue);
void push (flight_queue_t * queue, flight_t * flight);
flight_t * front (flight_queue_t * queue);
uint32_t size (flight_queue_t * queue);

atsim_time_t sim_ClockToTime(uint16_t clock);
uint32_t sim_TimeToClock(atsim_time_t time);

bool UpdateFlight (flight_t *flight, uint16_t timer);
void OutputFlightLog(flight_t *flight, uint16_t timer);

void init_airport(airport_t *airport);
bool RunwayReady(airport_t * airport, uint16_t FlightNumber);
void QueueDeparture(airport_t * airport, flight_t * flight);
void QueueArrival(airport_t * airport, flight_t * flight);
void EnterRunway(airport_t * airport, flight_t * flight);
void ManageRunway(airport_t * airport, uint16_t timer);
void UpdateAirport(airport_t * airport, uint16_t timer);
void ManageRunwayQueue(airport_t * airport, uint16_t timer);

#endif // AIRPORT_H
