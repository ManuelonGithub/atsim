/*
 * File: atsim.c
 * Author: Manuel Burnay
 * Date: May 20, 2019
 * Purpose:
 *      This file contains the main function of the project.
 *      It also contains functions that initialize and configure
 *      the parameters of the simulation.
 */

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <stdint.h>

#include "atsim_definitions.h"

const char IN_END[] = "end";

airport_t * find_airport(simulation_param_t *sim, const char *code);
void configure_simulation_data(simulation_param_t *sim, const char *data);
void sort_flights(flight_t *flight, uint16_t flight_count);

int main(int argc, char ** argv)
{
    // Initialize the simulation parameters.
    simulation_param_t sim = {
            .airport_count = 0,
            .flight_count  = 0,
            .clock         = UINT16_MAX,
            .state         = READ_FLIGHT_INFO,
            .complete      = false
    };

    char flight_data[FLIGHT_DATA_MAX_SIZE];

    while (!sim.complete) {
        switch (sim.state) {
            /*
             * READ_FLIGHT_INFO state
             *
             * In here, the system will process input from the console.
             * Next state is SIMULATE. It'll transition once the system captures
             * the IN_END command from console, which tells the system that
             * there are no more flight inputs and to start simulating.
             */
            case READ_FLIGHT_INFO: {
                fgets(flight_data, FLIGHT_DATA_MAX_SIZE, stdin);

                // Check if the end of the console input has been received
                if (!memcmp(flight_data, IN_END, sizeof(IN_END)-1)) {
                    sort_flights(sim.flights, sim.flight_count);
                    for (int i = 0; i < sim.airport_count; i++) {
                        init_airport(&sim.airports[i]);
                    }
                    sim.state = SIMULATE;
                }
                // Check to see if the input is at least the size of the
                // smallest possible size for a flight data input
                else if (strlen(flight_data) > FLIGHT_DATA_MIN_SIZE) {
                    configure_simulation_data(&sim, flight_data);
                }
            } break;

            /*
             * SIMULATE state
             *
             * In here, the system will simulate the airport scheduling system.
             * Next state is SIMULATION_COMPLETE. It'll transition once the
             * flights in the system are complete.
             */
            case SIMULATE: {
                // If a single flight still requires to update, the simulation's
                // state goes back to SIMULATE
                sim.state = SIMULATION_COMPLETE;

                for (int i = 0; i < sim.flight_count; i++) {
                    // Only keep simulating while there are still flights
                    // left to update
                    sim.state = (UpdateFlight(&sim.flights[i], sim.clock)) ?
                            SIMULATE : sim.state;
                }

                for (int i = 0; i < sim.airport_count; i++) {
                    ManageRunway(&sim.airports[i], sim.clock);
                }

                sim.clock++;
            } break;

            /*
             * SIMULATION_COMPLETE state
             *
             * It set the simulation complete flag, which will end the
             * simulation in the following program loop pass.
             */
            case SIMULATION_COMPLETE: {
                sim.complete = true;
            }break;
        }
    }
    return 0;
}

/*
 * brief: Looks for an airport with a given code, and instantiates an airport
 *        if not found.
 *
 * param:   [in] sim: simulation_param_t *
 *                  > Pointer to simulation parameters data type.
 *                  > uses and alters the array of airports and the
 *                    current airport count in the simulation.
 *
 *          [in] code: const char *
 *                   > Airport code
 *
 *          [out] airport_t *
 *                > pointer to airport element with corresponding code.
 */
airport_t * find_airport(simulation_param_t *sim, const char *code)
{
    int i = 0;
    bool airport_found = false;

    while (i < sim->airport_count && !airport_found) {
        airport_found = !memcmp(sim->airports[i].code, code, CODE_LENGTH);
        i += !airport_found;    // only increment if airport wasn't found
    }

    if (i == sim->airport_count) {
        memcpy(sim->airports[sim->airport_count].code, code, CODE_STR_SIZE);
        sim->airport_count++;
    }

    return &sim->airports[i];
}

/*
 * brief: Configures the simulation parameters based on the input received
 *        from the console.
 *
 * param:   [in] sim: simulation_param_t *
 *                  > Pointer to simulation parameters data type.
 *                  > uses and alters every element in the simulation
 *                        parameters except state and complete flag.
 *
 *          [in] data: const char *
 *                   > data string received from console.
 */
void configure_simulation_data(simulation_param_t *sim, const char *data)
{
    flight_t * flight = &(sim->flights[sim->flight_count]);
    char origin_code[CODE_STR_SIZE], dest_code[CODE_STR_SIZE];
    atsim_time_t time;

    sscanf(data, "%2s %hd %hd %3s %hhd:%hhd %hd %3s", flight->carrier,
           &flight->number, &flight->ID, origin_code, &time.hour, &time.minute,
           &flight->time.flight, dest_code);

    /*
     * The console input isn't checked for invalid data on purpose.
     * As this is simply a simulation of a system, it is the user's
     * responsibility to input valid data into the program.
     */

    flight->state          = STAND_BY;
    
    // The simulation time is converted into its equivalent clock value.
    flight->time.scheduled = sim_TimeToClock(time);
    flight->origin         = find_airport(sim, origin_code);
    flight->destination    = find_airport(sim, dest_code);

    // Set the simulation clock to start at the first departure of the
    // simulation, as to avoid needless loops of the program.
    sim->clock = (flight->time.scheduled < sim->clock) ?
                 flight->time.scheduled : sim->clock;
    sim->flight_count++;
}

/*
 * brief: Sorts the flight elements in the simulation based on their carrier ID
 *        and their flight number. Utilizes simple bubble sort.
 *
 * param:   [in] flight: flight_t *
 *                    > array of flight data types.
 *
 *          [in] flight_count: uint16_t
 *                           > Count of flights in the simulation.
 */
void sort_flights(flight_t *flight, uint16_t flight_count)
{
    flight_t temp;
    int carrier_cmp;

    for (int i = 0; i < flight_count-1; i++) {
        for (int j = 0; j < flight_count-i-1; j++) {
            carrier_cmp = memcmp(flight[j].carrier, flight[j+1].carrier,
                    CARRIER_ID_LENGTH);

            // If the carrier ID is larger than the next element or
            // If the carrier ID is equal, but the flight number is larger
            if (carrier_cmp > EQ ||
                (carrier_cmp == EQ && flight[j].number > flight[j+1].number)) {
                temp = flight[j];
                flight[j] = flight[j + 1];
                flight[j + 1] = temp;
            }
        }
    }
}

