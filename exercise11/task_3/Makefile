CC = gcc
CFLAGS += -std=c11 -Wall -Wextra -Wpedantic -pthread -g -Wno-return-local-addr

TARGET_LIB = malloc_spy

.PHONY: all
all: $(TARGET_LIB).o $(TARGET_LIB).so

$(TARGET_LIB).so:
	$(CC) -shared -o $(TARGET_LIB).so $(TARGET_LIB).o

$(TARGET_LIB).o:
	$(CC) -c $(CFLAGS) -fpic $(TARGET_LIB).c

.PHONY: clean
clean:
	$(RM) $(TARGET_LIB).so *.o vgcore.*
