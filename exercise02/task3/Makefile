CC=gcc
CFLAGS=-std=c11 -Wall -Wextra -Wpedantic -D_DEFAULT_SOURCE

.PHONEY: all
all: task3

.PHONEY: clean
clean:
	$(RM) task3

.PHONEY: run
run: all
	./task3 6 100000000

.PHONEY: task3
task3: task3.c
