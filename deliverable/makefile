# Targets & general dependencies
PROGRAM = atsim
HEADERS = airport.h queue.h simulation_definitions.h
OBJS = atsim.o 
OBJ_DIR = ./

# add your additional object files here
ADD_OBJS = airport.o queue.o

# compilers, linkers, utilities, and flags
CC = gcc
CFLAGS = -Wall -g -std=c11
COMPILE = $(CC) $(CFLAGS)
LINK = $(CC) $(CFLAGS) -o $@ 

# implicit rule to build .o from .c files
%.o: %.c $(HEADERS)
	$(COMPILE) -c -o $@ $<

.PHONY: all

# explicit rules
all: $(PROGRAM)

$(PROGRAM): $(OBJS) $(ADD_OBJS)
	$(LINK) $(OBJS) $(ADD_OBJS)

.PHONY: clean

clean: 
	rm -f *.o

zip:
	make clean
	rm -f atsim.zip
	zip atsim.zip *.c *.h makefile runme.sh
