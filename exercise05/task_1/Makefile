CFLAGS += -std=c11 -Wall -Wextra -Wpedantic -pthread -Wno-return-local-addr

TARGETA = service
TARGETB = scheduler

.PHONY: all
all: $(TARGETA) $(TARGETB)

.PHONY: clean
clean:
	$(RM) $(TARGETA) $(TARGETB) *.o vgcore.*
