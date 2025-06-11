CC = gcc
CFLAGS += -std=c11 -Wall -Wextra -Wpedantic -pthread -g -Wno-return-local-addr

TARGET_PRG = my_program
TARGET_LIB = some_library

.PHONY: all
all: $(TARGET_LIB).o $(TARGET_LIB).so $(TARGET_PRG).o $(TARGET_PRG)

$(TARGET_LIB).so:
	$(CC) -shared -o $(TARGET_LIB).so $(TARGET_LIB).o

$(TARGET_LIB).o:
	$(CC) -c $(CFLAGS) -fpic $(TARGET_LIB).c

# $(TARGET_PRG):
# 	$(CC) $(TARGET_PRG).o -static -lm -o $(TARGET_PRG)

# $(TARGET_PRG).o:
# 	$(CC) -c $(CFLAGS) -fpic $(TARGET_PRG).c

.PHONY: clean
clean:
	$(RM) $(TARGET_PRG).so $(TARGET_PRG) $(TARGET_LIB).so *.o vgcore.*
