CFLAGS = -std=c11 -Wall -Wextra -pedantic -O2 -g -D_DEFAULT_SOURCE 
LDFLAGS = -pthread

TARGETS = task_2

.PHONY:
all: $(TARGETS)

.PHONY:
clean:
	$(RM) $(TARGETS) *.o 

task_2: task_2.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

