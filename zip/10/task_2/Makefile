CPPFLAGS += -D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE -D_DARWIN_C_SOURCE -D_BSD_SOURCE
CFLAGS += -std=c11 -Wall -Wextra -pedantic -pthread -g -O3
LDFLAGS += -pthread

TARGETS ?= dummy_allocator

COMMON_OBJS = membench.o allocator_tests.o main.o

.PHONY: all
all: $(TARGETS)

.PHONY: clean
clean:
	$(RM) $(TARGETS) *.o *_global *_local

membench.o: membench.h
allocator_tests.o: allocator_tests.h
main.o: allocator.h

%_global.o: CPPFLAGS += -DTHREAD_LOCAL_ALLOCATOR=0
%_local.o: CPPFLAGS += -DTHREAD_LOCAL_ALLOCATOR=1

# Dummy allocator
dummy_allocator.o: allocator.h allocator_tests.h membench.h

# Free list allocator
free_list_allocator_%.o: free_list_allocator.c 
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

# Best fit allocator
best_fit_allocator_%.o: best_fit_allocator.c
	$(CC) $(CPPFLAGS)  $(CFLAGS) -c -o $@ $<

dummy_allocator: $(COMMON_OBJS)

free_list_allocator_global: $(COMMON_OBJS)
free_list_allocator_local: $(COMMON_OBJS)
best_fit_allocator_global: $(COMMON_OBJS)
best_fit_allocator_local: $(COMMON_OBJS)
