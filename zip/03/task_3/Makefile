CFLAGS += -std=c11 -Wall -Wextra -Wpedantic -pthread -Wno-return-local-addr
LDFLAGS += -pthread

TARGETA = task_3a
TARGETB = task_3b

.PHONY: all
all: $(TARGETA) $(TARGETB)

.PHONY: clean
clean:
	$(RM) $(TARGETA) $(TARGETB) *.o
