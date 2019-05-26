//
// Created by Manuel on 2019-05-21.
//

#include "flight.h"

bool UpdateFlight(flight_t *flight, uint16_t timer)
{
    bool retval = true;

    switch (flight->state) {
        case STAND_BY: {
            if (flight->time.scheduled == timer) {
                printf("[%d] Flight %d is now begin taxi'd.\n", timer, flight->number);
               flight->state = DEPARTURE_TAXI;
            }
        } break;

        case DEPARTURE_TAXI: {
            if ((flight->time.scheduled + TAXI_DURATION) == timer) {
                if (RunwayReady(flight->origin, flight->number)) {
                    EnterRunway(flight->origin, flight->number);
                    flight->time.departure = timer;
                    flight->state = EN_ROUTE;
                    printf("[%d] Flight %d used the runway and is now En Route.\n", timer, flight->number);
                }
                else {
                    QueueDeparture(flight->origin, flight->number);
                    flight->state = WAIT_TO_TAKEOFF;
                    printf("[%d] Runway is currently busy. Flight %d has been queued to depart.\n", timer, flight->number);
                }
            }
        } break;

        case WAIT_TO_TAKEOFF: {
            if (RunwayReady(flight->origin, flight->number)) {
                EnterRunway(flight->origin, flight->number);
                flight->time.departure = timer;
                flight->state = EN_ROUTE;
                printf("[%d] Flight %d used the runway and is now En Route.\n", timer, flight->number);
            }
        } break;

        case EN_ROUTE: {
            if ((flight->time.departure + flight->time.flight) == timer) {
                if (RunwayReady(flight->destination, flight->number)) {
                    EnterRunway(flight->destination, flight->number);
                    flight->time.arrival = timer;
                    flight->state = ARRIVAL_TAXI;
                    printf("[%d] Flight %d has landed.\n", timer, flight->number);
                }
                else {
                    QueueArrival(flight->destination, flight->number);
                    flight->state = WAIT_TO_LAND;
                    printf("[%d] Runway is currently busy. Flight %d has been queued to arrive.\n", timer, flight->number);
                }
            }
        } break;

        case WAIT_TO_LAND: {
            if (RunwayReady(flight->destination, flight->number)) {
                EnterRunway(flight->destination, flight->number);
                flight->time.arrival = timer;
                flight->state = ARRIVAL_TAXI;
                printf("[%d] Flight %d has landed.\n", timer, flight->number);
            }
        } break;

        case ARRIVAL_TAXI: {
            if ((flight->time.arrival + TAXI_DURATION) == timer) {
                flight->state = COMPLETE;
                printf("[%d] Flight %d has completed.\n", timer, flight->number);
            }
        } break;

        case COMPLETE: {
            retval = false;
        } break;
    }

    return retval;
}