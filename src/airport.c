//
// Created by Manuel on 2019-05-21.
//
#include <string.h>

#include "airport.h"

void init_queue (flight_queue_t * queue)
{
    queue->write_ptr = 0;
    queue->read_ptr = 0;
}

void pop (flight_queue_t * queue)
{
    queue->read_ptr++;
    queue->read_ptr &= FLIGHT_QUEUE_SIZE_MASK;
}

void push (flight_queue_t * queue, uint16_t FlightNumber)
{
    queue->buffer[queue->write_ptr] = FlightNumber;
    queue->write_ptr++;
    queue->write_ptr &= FLIGHT_QUEUE_SIZE_MASK;
}

uint16_t front (flight_queue_t * queue)
{
    return queue->buffer[queue->read_ptr];
}

uint32_t size (flight_queue_t * queue)
{
    return ((queue->write_ptr) - (queue->read_ptr)) & FLIGHT_QUEUE_SIZE_MASK;
}

void init_airport(airport_t * airport)
{
    init_queue(&airport->runway_queue);
    init_queue(&airport->arrivals_queue);
    init_queue(&airport->departures_queue);
    airport->runway_busy = false;
}

bool RunwayReady(airport_t * airport, uint16_t FlightNumber)
{
    return (!airport->runway_busy &&
        ((front(&airport->runway_queue) == FlightNumber) ||
            size(&airport->runway_queue) == 0));
}

void QueueDeparture(airport_t * airport, uint16_t FlightNumber)
{
    push(&airport->departures_queue, FlightNumber);
}

void QueueArrival(airport_t * airport, uint16_t FlightNumber)
{
    push(&airport->arrivals_queue, FlightNumber);
}

void EnterRunway(airport_t * airport, uint16_t FlightNumber)
{
    if (front(&airport->runway_queue) == FlightNumber) {
        pop(&airport->runway_queue);
    }

    airport->runway_busy = true;
}

void UpdateAirport(airport_t * airport)
{
    bool queue_required = true;

    while (queue_required) {
        queue_required = false;

        if (size(&airport->departures_queue) != 0) {
            queue_required = true;
            push(&airport->runway_queue, front(&airport->departures_queue));
            pop(&airport->departures_queue);
        }

        if (size(&airport->arrivals_queue) != 0) {
            queue_required = true;
            push(&airport->runway_queue, front(&airport->arrivals_queue));
            pop(&airport->arrivals_queue);
        }

        airport->runway_busy = false;
    }
}