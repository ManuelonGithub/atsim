# Targets & general dependencies
PROGRAM = atsim
HEADERS = airport.h atsim_definitions.h queue.h
OBJS = atsim.o airport.o queue.o
SRC = src/atsim.c src/airport.o src/queue.o
# add your additional object files here
ADD_OBJS = 

# compilers, linkers, utilities, and flags
CC = gcc
CFLAGS = -Wall -g -pthread
COMPILE = $(CC) $(CFLAGS)
LINK = $(CC) $(CFLAGS) -o $@ 

# implicit rule to build .o from .c files
%.o: %.c $(HEADERS)
	$(COMPILE) -c -o $@ $<


# explicit rules
all: $(PROGRAM)

$(PROGRAM): $(OBJS) $(ADD_OBJS)
	$(LINK) $(OBJS) $(ADD_OBJS) -l pthread

atsimg: atsim.o airport.o queue.o
	$(LINK) atsim.o airport.o queue.o -l pthread

lib: atcsim_gold.o 
	 ar -r libatcsim.a atcsim_gold.o 

clean:
	rm -f *.o

zip:
	make clean
	rm -f atsim2.zip
	zip atsim2.zip $(SRC)
