/*
 * File: airport.h
 * Author: Manuel Burnay
 * Date: May 20, 2019
 * Purpose:
 *      This file contains the declarations and definitions of the
 *      structures, constants and functions used by the flight and airport
 *      elements in the simulation or used to manage them.
 *
 */


#ifndef ATSIM_AIRPORT_H
#define ATSIM_AIRPORT_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "queue.h"

#define EMPTY_QUEUE 0u
#define QUEUE_TYPE_MASK 1u
#define NEXT_QUEUE_TYPE(queue) ((queue + 1u) & QUEUE_TYPE_MASK)

typedef struct {
    uint8_t hour;
    uint8_t minute;
} atsim_time_t;

#define TAXI_DURATION   10u

atsim_time_t sim_ClockToTime(uint16_t clock);
uint32_t sim_TimeToClock(atsim_time_t time);

bool UpdateFlight (flight_t *flight, uint16_t timer);
void OutputFlightLog(flight_t *flight, uint16_t timer);

void init_airport(airport_t *airport);
void QueueDeparture(airport_t * airport, flight_t * flight);
void QueueArrival(airport_t * airport, flight_t * flight);
void ManageRunway(airport_t *airport, uint16_t sim_clock);

#endif // ATSIM_AIRPORT_H
