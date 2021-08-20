CC = gcc
LFLAGS = -Wall -fopenmp
SRC := $(wildcard *.c)
COUT := $(patsubst %.c,%.out, $(SRC))

main: $(COUT)

%.out: %.c
	$(CC) $(LFLAGS) $< -o $(@F)
