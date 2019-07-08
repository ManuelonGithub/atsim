# Targets & general dependencies
PROGRAM = atsim
SRC_DIR = src
INC_DIR = includes

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/.c=.o)

# compilers, linkers, utilities, and flags
CC = gcc
CFLAGS = -Wall -g -std=c11 -I$(INC_DIR)
COMPILE = $(CC) $(CFLAGS)
LINK = $(CC) $(CFLAGS) -o $@ 

$%.o: $(SRC_DIR)/%.c
	$(COMPILE) $< -o $@

.PHONY: all

all: $(PROGRAM)

$(PROGRAM): $(OBJ) 
	$(LINK) $(OBJ) 

.PHONY: clean

clean:
	make all
	rm -f *.o

zip:
	make clean
	rm -f atsim.zip
	zip atsim.zip -r *
