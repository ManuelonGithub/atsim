#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <stdint.h>

#include "simulation_definitions.h"

const char IN_END[] = "end";

airport_t * FindAirport(simulation_param_t *sim, const char *code)
{
    int i = 0;
    bool AirportFound = false;

    while (i < sim->airport_count && !AirportFound) {
        AirportFound = !memcmp(sim->airports[i].code, code, CODE_LENGTH);
        i += !AirportFound;
    }

    if (i == sim->airport_count) {
        memcpy(sim->airports[sim->airport_count].code, code, CODE_STR_SIZE);
        sim->airport_count++;
    }

    return &sim->airports[i];
}

void ConfigureSimulationData(simulation_param_t *sim, const char *data)
{
    flight_t * flight = &(sim->flights[sim->flight_count]);
    char origin_code[ID_LENGTH], dest_code[ID_LENGTH];
    atsim_time_t time;

    sscanf(data, "%2s %hd %hd %3s %hhd:%hhd %hd %3s", flight->carrier,
            &flight->number, &flight->ID, origin_code, &time.hour, &time.minute,
            &flight->time.flight, dest_code);

    flight->state          = STAND_BY;
    flight->time.scheduled = sim_TimeToClock(time);
    flight->origin         = FindAirport(sim, origin_code);
    flight->destination    = FindAirport(sim, dest_code);

    sim->clock = (flight->time.scheduled < sim->clock) ?
            flight->time.scheduled : sim->clock;
    sim->flight_count++;
}

void SortFlights(flight_t *flight, uint16_t flight_count)
{
    flight_t TempFlight;

    for (int i = 0; i < flight_count-1; i++) {
        for (int j = 0; j < flight_count-i-1; j++) {
            if (flight[j].number > flight[j+1].number) {
                TempFlight = flight[j];
                flight[j] = flight[j + 1];
                flight[j + 1] = TempFlight;
            }
        }
    }

    for (int i = 0; i < flight_count-1; i++) {
        printf("%d, ", flight[i].number);
    }
    printf("%d \n", flight[flight_count-1].number);
}


void OutputSimulationResults(simulation_param_t *sim)
{
    atsim_time_t CompletionTime, ScheduleTime;
    flight_t * flight;
    uint16_t delay;

    for (int i = 0; i < sim->flight_count; i++) {
        flight = &sim->flights[i];
        CompletionTime = sim_ClockToTime(flight->time.arrival);
        ScheduleTime = sim_ClockToTime(flight->time.scheduled);
        delay = flight->time.arrival - flight->time.scheduled
                - flight->time.flight - 2*TAXI_DURATION;

        printf("[%02d:%02d] %s %d from %s to %s, departed %02d:%02d, delay %d.\n",
                CompletionTime.hour, CompletionTime.minute, flight->carrier,
                flight->number, flight->origin->code, flight->destination->code,
                ScheduleTime.hour, ScheduleTime.minute, delay);
    }
}

int main(int argc, char ** argv)
{
    simulation_param_t sim = {
            .airport_count = 0,
            .flight_count  = 0,
            .clock         = UINT16_MAX,
            .state         = READ_FLIGHT_INFO,
            .complete      = false
    };

    char FlightData[FLIGHT_DATA_MAX_SIZE];

    while (!sim.complete) {
        switch (sim.state) {
            case READ_FLIGHT_INFO: {
                fgets(FlightData, FLIGHT_DATA_MAX_SIZE, stdin);

                if (!memcmp(FlightData, IN_END, sizeof(IN_END)-1)) {
                    SortFlights(sim.flights, sim.flight_count);
                    for (int i = 0; i < sim.airport_count; i++) {
                        init_airport(&sim.airports[i]);
                    }
                    sim.state = SIMULATE;
                }
                else if (strlen(FlightData) > FLIGHT_DATA_MIN_SIZE){
                    ConfigureSimulationData(&sim, FlightData);
                }
            } break;

            case SIMULATE: {
                sim.state = SIMULATION_COMPLETE;

                for (int i = 0; i < sim.flight_count; i++) {
                    sim.state = (UpdateFlight(&sim.flights[i], sim.clock)) ?
                            SIMULATE : sim.state;
                }

                for (int i = 0; i < sim.airport_count; i++) {
                    UpdateAirport(&sim.airports[i]);
                }

                sim.clock++;
            } break;

            case SIMULATION_COMPLETE: {
                sim.complete = true;
            }break;
        }
    }
    return 0;
}

