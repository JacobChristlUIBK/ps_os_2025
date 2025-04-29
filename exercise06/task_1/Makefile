CFLAGS += -std=c11 -Wall -Wextra -Wpedantic -pthread -Wno-return-local-addr

TARGETA = task_1_atomic
TARGETB = task_1_mutex

.PHONY: all
all: $(TARGETA) $(TARGETB)

.PHONY: clean
clean:
	$(RM) $(TARGETA) $(TARGETB) *.o vgcore.*
