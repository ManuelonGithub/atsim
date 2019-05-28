//
// Created by Manuel on 2019-05-21.
//

#include "airport.h"

bool UpdateFlight(flight_t *flight, uint16_t timer)
{
    bool retval = true;

    switch (flight->state) {
        case STAND_BY: {
            if (flight->time.scheduled == timer) {
//                 printf("[%d:%d] Flight %d is now begin taxi'd.\n", sim_ClockToTime(timer).hour, sim_ClockToTime(timer).minute, flight->number);
               flight->state = DEPARTURE_TAXI;
            }
        } break;

        case DEPARTURE_TAXI: {
            if ((flight->time.scheduled + TAXI_DURATION) == timer) {
                QueueDeparture(flight->origin, flight);
                flight->state = WAIT_TO_TAKEOFF;
//                 printf("[%d:%d] Flight %d has finished the departure taxi and has been queued for departure.\n", sim_ClockToTime(timer).hour, sim_ClockToTime(timer).minute, flight->number);
            }
        } break;

        case WAIT_TO_TAKEOFF: {
//            printf("[%d:%d] Flight %d is waiting to depart.\n", sim_ClockToTime(timer).hour, sim_ClockToTime(timer).minute, flight->number);
        } break;

        case EN_ROUTE: {
            if ((flight->time.departure + flight->time.flight) == timer) {
                QueueArrival(flight->destination, flight);
                flight->state = WAIT_TO_LAND;
//                printf("[%d:%d] Flight %d has completed the trip and has been queued for landing.\n", sim_ClockToTime(timer).hour, sim_ClockToTime(timer).minute, flight->number);
            }
        } break;

        case WAIT_TO_LAND: {
//            printf("[%d:%d] Flight %d is waiting to land.\n", sim_ClockToTime(timer).hour, sim_ClockToTime(timer).minute, flight->number);
        } break;

        case ARRIVAL_TAXI: {
            if ((flight->time.land + TAXI_DURATION) == timer) {
                flight->time.arrival = timer;
                OutputFlightLog(flight, timer);
                flight->state = COMPLETE;
//                 printf("[%d:%d] Flight %d has completed.\n", sim_ClockToTime(timer).hour, sim_ClockToTime(timer).minute, flight->number);
            }
        } break;

        case COMPLETE: {
            retval = false;
        } break;
    }

    return retval;
}

void OutputFlightLog(flight_t *flight, uint16_t timer)
{
    atsim_time_t CompletionTime, ScheduleTime;
    uint16_t delay;

    CompletionTime = sim_ClockToTime(timer);
    ScheduleTime = sim_ClockToTime(flight->time.scheduled);
    delay = flight->time.arrival - flight->time.scheduled
            - flight->time.flight - 2*TAXI_DURATION;

    printf("[%02d:%02d] %s %d from %s to %s, departed %02d:%02d, delay %d.\n",
           CompletionTime.hour, CompletionTime.minute, flight->carrier,
           flight->number, flight->origin->code, flight->destination->code,
           ScheduleTime.hour, ScheduleTime.minute, delay);
}


atsim_time_t sim_ClockToTime(uint16_t clock)
{
    return (atsim_time_t) {.hour = clock / 60, .minute = clock % 60};
}

uint32_t sim_TimeToClock(atsim_time_t time)
{
    return ((time.hour * 60) + time.minute);
}