CC=gcc
CFLAGS=-Wall -std=c99 -g

all: ll_cycle bork vector bit_ops

ll_cycle: ll_cycle.c test_ll_cycle.c ll_cycle.h
	$(CC) $(CFLAGS) -o ll_cycle ll_cycle.c test_ll_cycle.c

bork: bork.c 
	$(CC) $(CFLAGS) -o bork bork.c

vector: vector.c test_vector.c vector.h
	$(CC) $(CFLAGS) -o vector vector.c test_vector.c

bit_ops: bit_ops.c test_bit_ops.c bit_ops.h
	$(CC) $(CFLAGS) -o bit_ops bit_ops.c test_bit_ops.c

clean:
	rm -f ll_cycle bork vector bit_ops
