CC=gcc
CFLAGS=-std=c11 -Wall -Wextra -Wpedantic -D_DEFAULT_SOURCE

.PHONEY: all
all: task_1

.PHONEY: clean
clean:
	$(RM) task_1

.PHONEY: run
run: all
	./task_1 6 100000000

.PHONEY: task_1
task_1: task_1.c
