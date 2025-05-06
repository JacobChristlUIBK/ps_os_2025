CFLAGS = -std=c11 -Wall -Wextra -pedantic -O2 -g -pthread
LDFLAGS = -pthread

TARGETS = slot_machine.o task_3a.o task_3.o task_3a task_3

.PHONY:
all: $(TARGETS)

.PHONY:
clean:
	$(RM) $(TARGETS) *.o

task_3: task_3.o slot_machine.o
slot_machine.o: slot_machine.h slot_machine.c
task_3.o: task_3.c slot_machine.h
	$(CC) $(CFLAGS) $(LDFLAGS) -DTHREAD_PER_JOB -c -o $@ $<

task_3a: task_3a.o slot_machine.o thread_pool.o
thread_pool.o: thread_pool.c thread_pool.h
task_3a.o: task_3.c slot_machine.h  thread_pool.h
	$(CC) $(CFLAGS) $(LDFLAGS) -DTHREAD_POOL -c -o $@ $<

