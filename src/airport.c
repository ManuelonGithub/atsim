/**
 * @file    airport.c
 * @author  Manuel Burnay
 * @date    May 20, 2019
 * @details This file contains the function bodies that initialize
 *          and manage the airports and flights in the simulation.
 */

#include "airport.h"

/**
 * @brief   Initializes the queues in the airport and gives a default value for
 *          the last queueing done by the airport.
 * @param   [out] airport: airport_t*
 *                         -- Pointer to an airport element to be initialized.
 */
bool init_airport(airport_t *airport)
{
    airport->last_queue_type = DEPARTURE;
    return (
            init_queue(&airport->arrivals_queue) == 0 &&
            init_queue(&airport->departures_queue) == 0
    );
}

/**
 * @brief   Deinitializes an airport element.
 * @param   [out] airport: airport_t*
 *                         -- Pointer to an airport element to be deinitialized.
 * @return  [bool]
 *          -- True if the deinitialization was successfully completed,
 *          False if mutex destroy calls for either of the airport's queues
 *          failed.
 */
bool deinit_airport(airport_t *airport)
{
    return (
            pthread_mutex_destroy(&airport->arrivals_queue.mutex) == 0 &&
            pthread_mutex_destroy(&airport->departures_queue.mutex) == 0
    );
}

/**
 * @brief Wrapper function for enqueueing flights into an airport's
 *        Departure queue.
 * @param [out] airport: airport_t *
 *                       -- Pointer to an airport element
 *                          with the departure queue.
 * @param [in] flight: flight_t *
 *                     -- Pointer to the flight element to be queued.
 */
void queue_departure(airport_t *airport, flight_t *flight)
{
    enqueue(&airport->departures_queue, flight);
}

/**
 * @brief   Wrapper function for enqueueing flights into an airport's
 *          Arrival queue.
 * @param   [out] airport: airport_t *
 *                         -- Pointer to an airport element
 *                            with the arrival queue.
 * @param   [in] flight: flight_t *
 *                       -- Pointer to the flight element to be queued.
 */
void queue_arrival(airport_t *airport, flight_t *flight)
{
    enqueue(&airport->arrivals_queue, flight);
}

/**
 * @brief   Manages the runway of an airport.
 * @param   [in, out] airport: airport_t *
 *                             -- Pointer to an airport element to be managed.
 * @param   [in] sim_clock: uint16_t
 *                          -- Current simulation clock tick.
 * @details This the queueing management system of the airport queues.
 *          It'll pick a flight to enter the runway (i.e. progress /
 *          move to the next state) based on currently queued flights and the
 *          last queueing type made by the airport.
 */
void manage_runway(airport_t *airport, uint16_t sim_clock)
{
    // Grouping the two queues temporarily to an array allows the queueing
    // of flights to be done with less flow control operations.
    flight_queue_t *queues[QUEUE_TYPES] = {
            &airport->departures_queue,
            &airport->arrivals_queue
    };

    queue_types_t CurrentQueue = NEXT_QUEUE_TYPE(airport->last_queue_type);
    flight_t * frontFlight = NULL;

    // Making this into a loop would be just as code and processing heavy.
    if (size(queues[CurrentQueue]) != EMPTY_QUEUE) {
        frontFlight = peek(queues[CurrentQueue]);
        dequeue(queues[CurrentQueue]);
    }
    else {
        CurrentQueue = NEXT_QUEUE_TYPE(CurrentQueue);
        if (size(queues[CurrentQueue]) != EMPTY_QUEUE) {
            frontFlight = peek(queues[CurrentQueue]);
            dequeue(queues[CurrentQueue]);
        }
    }

    // If a flight to be queued was found.
    if (frontFlight != NULL) {
        if (frontFlight->state == WAIT_TO_TAKEOFF) {
            frontFlight->state = EN_ROUTE;
            frontFlight->plane->airport = PLANE_ON_AIR;
            frontFlight->time.departure = sim_clock;
        }
        else if (frontFlight->state == WAIT_TO_LAND) {
            frontFlight->state = ARRIVAL_TAXI;
            frontFlight->time.arrival = sim_clock;
        }

        airport->last_queue_type = CurrentQueue;
    }
}

/**
 * @brief   Updates the flight's progression in the simulation.
 * @param   [in, out] flight: flight_t *
 *                            -- Pointer to the flight element to be updated.
 * @param   [in] sim_clock: uint16_t
 *                          --  Current simulation clock tick.
 * @return  bool
 *          -- True if flight still requires to be updated, False if not.
 */
bool update_flight(flight_t *flight, uint16_t sim_clock)
{
    bool retval = true;

    switch (flight->state) {
        /*
         * STAND_BY state
         * The flight's progression will stall here while it's scheduled time
         * hasn't been reach by the simulation yet.
         *
         * Next State #2: DEPARTURE_TAXI
         * It will transition once the simulation has reached the flight's
         * scheduled time and its assigned plane is ready to be used.
         */
        case STAND_BY: {
            if (flight->time.scheduled <= sim_clock) {
                if (plane_ready(flight)) {
                    flight->state = DEPARTURE_TAXI;
                    flight->time.departure = sim_clock;
                }
            }
        } break;

        /*
         * DEPARTURE_TAXI state
         * The flight's progression will stall here while the departure taxi
         * procedure period hasn't elapsed.
         * When the period has elapsed, the flight will queue itself into its
         * origin airport's departure queue.
         *
         * Next State: WAIT_TO_TAKEOFF
         * It will transition once the the taxi procedure period has elapsed.
         */
        case DEPARTURE_TAXI: {
            if ((flight->time.departure + TAXI_DURATION) == sim_clock) {
                queue_departure(flight->origin, flight);
                flight->state = WAIT_TO_TAKEOFF;
            }
        } break;

        /*
         * WAIT_TO_TAKEOFF state
         * The flight's progression will stall here until its origin airport
         * instructs the flight to use the runway
         * (occurs in the manage_runway function).
         *
         * Next State: EN_ROUTE
         * It will transition once the origin airport has instructed the
         * flight to use the runway.
         */
        case WAIT_TO_TAKEOFF: {
        } break;

        /*
         * EN_ROUTE state
         * The flight progression remains here while the flight duration period
         * hasn't elapsed.
         * When the period has elapsed, the flight will queue itself into its
         * destination airport's arrival queue.
         *
         * Next State: WAIT_TO_LAND
         * It will transition once the the flight duration period has elapsed.
         */
        case EN_ROUTE: {
            if ((flight->time.departure + flight->time.flight) == sim_clock) {
                queue_arrival(flight->destination, flight);
                flight->state = WAIT_TO_LAND;
            }
        } break;

        /*
         * WAIT_TO_LAND state
         * The flight's progression will stall here until its destination
         * airport instructs the flight to use the runway
         * (occurs in the manage_runway function).
         *
         * Next State: ARRIVAL_TAXI
         * It will transition once the destination airport has instructed the
         * flight to use the runway.
         */
        case WAIT_TO_LAND: {
        } break;

        /*
         * ARRIVAL_TAXI state
         * The flight's progression will stall here while the departure taxi
         * procedure period hasn't elapsed.
         * When the period has elapsed, flight log will be outputted to the
         * console.
         *
         * Next State: COMPLETE
         * It will transition once the the taxi procedure period has elapsed.
         */
        case ARRIVAL_TAXI: {
            if ((flight->time.arrival + TAXI_DURATION) == sim_clock) {
                flight->time.arrival = sim_clock;
                flight->state = COMPLETE;

                flight->plane->airport = flight->destination;
                flight->plane->groom = PLANE_GROOM_DURATION;
            }
        } break;

        /*
         * COMPLETE state
         * The flight progression is complete and so the function will return
         * the value indicating as such.
         */
        case COMPLETE: {
            retval = false;
        } break;
    }

    return retval;
}

/**
 * @brief   Checks if the plane is ready to be assigned to a flight.
 * @param   [in] flight: flight_t*
 *          -- Pointer to the flight that is checking for its plane to be ready.
 * @details When a flight becomes 'En Route', the plane no longer has an airport
 *          assigned to it, which is how it's determined whether or not a plane
 *          is 'busy' with another flight.
 * @return  bool
 *          -- True if the plane is ready to be assigned,
 *             False if not.
 */
bool plane_ready(flight_t *flight)
{
    plane_t *plane = flight->plane;
    return (plane->airport == flight->origin && plane->groom == 0);
}

/**
 * @brief   Outputs the flight log once the flight has finished it's progression.
 * @param   [in] flight: flight_t *
 *                       -- Pointer to the flight element to be updated.
 * @param   [in] sim_clock: uint16_t
 *                          -- Current simulation clock tick.
 */
void output_flight_log(flight_t *flight)
{
    atsim_time_t CompletionTime, ScheduleTime;
    uint16_t delay;

    CompletionTime = sim_ClockToTime(flight->time.arrival);
    ScheduleTime = sim_ClockToTime(flight->time.scheduled);
    delay = flight->time.arrival - flight->time.scheduled
            - flight->time.flight - 2*TAXI_DURATION;

    printf("[%02d:%02d] %s %d from %s to %s, departed %02d:%02d, delay %d.\n",
           CompletionTime.hour, CompletionTime.minute, flight->carrier,
           flight->number, flight->origin->code, flight->destination->code,
           ScheduleTime.hour, ScheduleTime.minute, delay);
}

/**
 * @brief   Converts the simulation clock into simulation time.
 * @param   [in] clock: uint16_t
 *                      -- Simulation clock value.
 * @details To simplify the simulation's logic, the simulation times inside each
 *          flight are converted into a value that can be incremented easily
 *          without requiring overflow checking.
 *          This function then converts the simulation clock into its equivalent
 *          hour and minute time.
 * @return  atsim_time_t
 *          -- struct with the equivalent hour and minute
 *             to the simulation clock.
 */
atsim_time_t sim_ClockToTime(uint16_t clock)
{
    return (atsim_time_t) {.hour = clock / 60, .minute = clock % 60};
}

/**
 * @brief   Converts the simulation time into simulation clock.
 * @param   [in] time: atsim_time_t
 *                     -- Simulation time data type.
 * @details To simplify the simulation's logic, the simulation times inside each
 *          flight are converted into a value that can be incremented easily
 *          without requiring overflow checking.
 *          This function then converts the simulation time into its equivalent
 *          simulation clock value.
 * @return  uint32_t
 *          -- Simulation clock value.
 */
uint32_t sim_TimeToClock(atsim_time_t time)
{
    return ((time.hour * 60) + time.minute);
}