//
// Created by Manuel on 2019-05-21.
//

#ifndef ASSIGNMENT_1_AIRPORT_H
#define ASSIGNMENT_1_AIRPORT_H

#include <stdio.h>
#include <stdbool.h>

#define FLIGHT_QUEUE_SIZE   256u
#define FLIGHT_QUEUE_SIZE_MASK  FLIGHT_QUEUE_SIZE-1u

#define CODE_LENGTH     3
#define CODE_STR_SIZE   CODE_LENGTH+1

typedef struct {
    uint32_t    read_ptr;
    uint32_t    write_ptr;
    uint16_t    buffer[FLIGHT_QUEUE_SIZE];
} flight_queue_t;

typedef struct {
    flight_queue_t  departures_queue;
    flight_queue_t  arrivals_queue;
    flight_queue_t  runway_queue;
    char            code[CODE_STR_SIZE];
    bool            runway_busy;
} airport_t;

void init_queue (flight_queue_t * queue);
void pop (flight_queue_t * queue);
void push (flight_queue_t * queue, uint16_t FlightNumber);
uint16_t front (flight_queue_t * queue);
uint32_t size (flight_queue_t * queue);

void init_airport(airport_t *airport);
bool RunwayReady(airport_t * airport, uint16_t FlightNumber);
void QueueDeparture(airport_t * airport, uint16_t FlightNumber);
void QueueArrival(airport_t * airport, uint16_t FlightNumber);
void EnterRunway(airport_t * airport, uint16_t FlightNumber);
void UpdateAirport(airport_t * airport);

#endif //ASSIGNMENT_1_AIRPORT_H
