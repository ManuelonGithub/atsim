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

void push (flight_queue_t * queue, flight_t * flight)
{
    queue->buffer[queue->write_ptr] = flight;
    queue->write_ptr++;
    queue->write_ptr &= FLIGHT_QUEUE_SIZE_MASK;
}

flight_t* front (flight_queue_t * queue)
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
    airport->last_queue_type = DEPARTURE;
}

void QueueDeparture(airport_t * airport, flight_t * flight)
{
    push(&airport->departures_queue, flight);
}

void QueueArrival(airport_t * airport, flight_t * flight)
{
    push(&airport->arrivals_queue, flight);
}

void ManageRunway(airport_t * airport, uint16_t timer)
{
    flight_t * frontFlight = front(&airport->runway_queue);

    if (frontFlight == NULL) {
        return;
    }

    if (frontFlight->state == WAIT_TO_TAKEOFF) {
        frontFlight->state = EN_ROUTE;
        frontFlight->time.departure = timer;
//        printf("[%d:%d] Flight %d has taken off from airport %s.\n", sim_ClockToTime(timer).hour, sim_ClockToTime(timer).minute, front(&airport->runway_queue)->number, airport->code);
    }
    else if (frontFlight->state == WAIT_TO_LAND) {
        frontFlight->state = ARRIVAL_TAXI;
        frontFlight->time.land = timer;
//        printf("[%d:%d] Flight %d has landed at airport %s.\n", sim_ClockToTime(timer).hour, sim_ClockToTime(timer).minute, front(&airport->runway_queue)->number, airport->code);
    }

    pop(&airport->runway_queue);
}

void ManageRunwayQueue(airport_t * airport, uint16_t timer)
{
    flight_queue_t *queues[QUEUE_TYPES] = {&airport->departures_queue, &airport->arrivals_queue};
    queue_types_t CurrentQueue = NEXT_QUEUE_TYPE(airport->last_queue_type);
    bool queue_required = true;

    if (size(queues[CurrentQueue]) == EMPTY_QUEUE) {
        CurrentQueue = NEXT_QUEUE_TYPE(CurrentQueue);
        if (size(queues[CurrentQueue]) == EMPTY_QUEUE) {
            queue_required = false;
        }
    }

    while (queue_required) {
        push(&airport->runway_queue, front(queues[CurrentQueue]));
//        printf("[%d:%d] Flight %d has been queued for the runway of airport %s. Runway use: ", sim_ClockToTime(timer).hour, sim_ClockToTime(timer).minute, front(queues[CurrentQueue])->number, airport->code);
//        if (CurrentQueue == DEPARTURE) printf("Departure.\n");
//        else printf("Arrival.\n");
        pop(queues[CurrentQueue]);
        airport->last_queue_type = CurrentQueue;

        CurrentQueue = NEXT_QUEUE_TYPE(CurrentQueue);

        if (size(queues[CurrentQueue]) == EMPTY_QUEUE) {
            CurrentQueue = NEXT_QUEUE_TYPE(CurrentQueue);
            if (size(queues[CurrentQueue]) == EMPTY_QUEUE) {
                queue_required = false;
            }
        }
    }

    ManageRunway(airport, timer);
}