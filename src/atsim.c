#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <stdint.h>

#include "simulation_definitions.h"

const char IN_END[] = "END";

atsim_time_t sim_ClockToTime(uint16_t clock);
uint32_t sim_TimeToClock(atsim_time_t time);

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
    char origin_code[CODE_STR_SIZE], dest_code[CODE_STR_SIZE];
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

atsim_time_t sim_ClockToTime(uint16_t clock)
{
    return (atsim_time_t) {.hour = clock / 60, .minute = clock % 60};
}

uint32_t sim_TimeToClock(atsim_time_t time)
{
    return ((time.hour * 60) + time.minute);
}