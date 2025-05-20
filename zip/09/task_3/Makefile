CFLAGS += -std=c11 -Wall -Wextra -Wpedantic -pthread -Wno-return-local-addr -D_DEFAULT_SOURCE -g

TARGET = task_3
MY_BARRIER = my_pthread_barrier

.PHONY: all
all:
	$(CC) $(CFLAGS) ./$(TARGET).c -c -o ./$(TARGET).o
	$(CC) $(CFLAGS) ./$(MY_BARRIER).c -c -o ./$(MY_BARRIER).o
	$(CC) $(CFLAGS) ./$(TARGET).o ./$(MY_BARRIER).o -o ./$(TARGET)

.PHONY: clean
clean:
	$(RM) $(TARGET) $(MY_BARRIER) *.o vgcore.*
