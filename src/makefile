# Targets & general dependencies
PROGRAM = atsim
HEADERS = airport.h flight.h simulation_definitions.h
OBJS = atsim.o 
OBJ_DIR = ./

# add your additional object files here
ADD_OBJS = airport.o flight.o

# compilers, linkers, utilities, and flags
CC = gcc
CFLAGS = -Wall -g -std=c11
COMPILE = $(CC) $(CFLAGS)
LINK = $(CC) $(CFLAGS) -o $@ 

# implicit rule to build .o from .c files
%.o: %.c $(HEADERS)
	$(COMPILE) -c -o $@ $<


# explicit rules
all: $(PROGRAM)

$(PROGRAM): $(OBJS) $(ADD_OBJS)
	$(LINK) $(OBJS) $(ADD_OBJS)

clean:
	rm -f *.o $(PROGRAM) atsim

zip:
	make clean
	rm -f atsim.zip
	zip atsim.zip atsim.c makefile runme.sh
