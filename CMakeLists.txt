cmake_minimum_required(VERSION 3.10)
project(atsim C)

set(CMAKE_C_STANDARD 11)
set(EXECUTABLE_OUTPUT_PATH ./bin)

include_directories(src)

add_executable(atsim src/airport.c src/atsim.c src/queue.c src/queue.h)
