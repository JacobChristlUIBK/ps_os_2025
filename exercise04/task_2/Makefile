CFLAGS += -std=c11 -Wall -Wextra -Wpedantic -pthread -Wno-return-local-addr

TARGETA = cat_cut
TARGETB = cat_grep_cut

.PHONY: all
all: $(TARGETA) $(TARGETB)

.PHONY: clean
clean:
	$(RM) $(TARGETA) $(TARGETB) *.o vgcore.*
