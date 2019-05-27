//
// Created by Manuel on 2019-05-21.
//

#ifndef ASSIGNMENT_1_AIRPORT_H
#define ASSIGNMENT_1_AIRPORT_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "flight.h"

#define FLIGHT_QUEUE_SIZE   256u
#define FLIGHT_QUEUE_SIZE_MASK  FLIGHT_QUEUE_SIZE-1u

typedef struct {
    uint32_t                read_ptr;
    uint32_t                write_ptr;
    struct flight_struct *  buffer[FLIGHT_QUEUE_SIZE];
} flight_queue_t;

#define CODE_LENGTH     3
#define CODE_STR_SIZE   CODE_LENGTH+1

typedef struct {
    flight_queue_t  departures_queue;
    flight_queue_t  arrivals_queue;
    flight_queue_t  runway_queue;
    char            code[CODE_STR_SIZE];
    bool            runway_busy;
} airport_t;

typedef struct {
    uint8_t hour;
    uint8_t minute;
} atsim_time_t;

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

typedef struct flight_struct {
    char            carrier[ID_LENGTH];
    uint16_t        number;
    uint16_t        ID;
    airport_t *     origin;
    airport_t *     destination;
    flight_times_t  time;
    flight_states_t state;
} flight_t;

#define TAXI_DURATION   10u


void init_queue (flight_queue_t * queue);
void pop (flight_queue_t * queue);
void push (flight_queue_t * queue, uint16_t FlightNumber);
uint16_t front (flight_queue_t * queue);
uint32_t size (flight_queue_t * queue);

atsim_time_t sim_ClockToTime(uint16_t clock);
uint32_t sim_TimeToClock(atsim_time_t time);

bool UpdateFlight (flight_t *flight, uint16_t timer);
void OutputFlightLog(flight_t *flight, uint16_t timer);

void init_airport(airport_t *airport);
bool RunwayReady(airport_t * airport, uint16_t FlightNumber);
void QueueDeparture(airport_t * airport, uint16_t FlightNumber);
void QueueArrival(airport_t * airport, uint16_t FlightNumber);
void EnterRunway(airport_t * airport, flight_t * flight);
void UpdateAirport(airport_t * airport);

#endif //ASSIGNMENT_1_AIRPORT_H
