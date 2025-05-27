#include <assert.h>
#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include "allocator.h"
#include "allocator_tests.h"
#include "bits/pthreadtypes.h"
#include "membench.h"

#define BLOCK_SIZE 1024
#define HEADER_BLOCK_SIZE (1024 + sizeof(header_t))

// Holds the header of a block, pointing to the next free block.
typedef struct header {
	void* next;
} header_t;

_Thread_local static void* pool = NULL;
_Thread_local static size_t pool_size = 0;
_Thread_local static header_t header_start = { .next = NULL };

// initializes a new pool
void my_allocator_init(size_t total_size) {
	int block_count = 0;
	size_t true_size = total_size - (total_size % HEADER_BLOCK_SIZE);
	size_t remaining_space = true_size;

	// return if pool has already been initialized
	if (pool != NULL) {
		return;
	}

	// map pool
	pool = mmap(NULL, true_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (pool == MAP_FAILED) {
		pool = NULL;
		return;
	}

	header_start.next = pool;
	header_t* cursor = &header_start;

	// populate pool with headers
	while (cursor) {
		// if this is the last block to be initialized, point to next header, otherwise NULL
		cursor->next = remaining_space / HEADER_BLOCK_SIZE > 1
		                        ? (void*)((unsigned long)pool + (HEADER_BLOCK_SIZE * (block_count + 1)))
		                        : NULL;

		cursor = cursor->next;

		block_count++;
		remaining_space -= HEADER_BLOCK_SIZE;
	}

	pool_size = true_size;
}

void my_allocator_destroy(void) {
	if (!pool) {
		return;
	}

	int res;

	res = munmap(pool, pool_size);
	if (res == -1) {
		return;
	}

	pool = NULL;
	pool_size = 0;
	header_start.next = NULL;
}

void* my_malloc(size_t size) {
	header_t* block = NULL;

	if (!pool) {
		return NULL;
	}

	if (size > BLOCK_SIZE) {
		return NULL;
	}

	if (!header_start.next) {
		return NULL;
	}

	block = header_start.next;
	header_start.next = block->next;

	return (unsigned char*)block + sizeof(header_t);
}

void my_free(void* ptr) {
	header_t* header_to_free = NULL;
	header_t* header_cursor = NULL;

	if (!pool) {
		return;
	}

	header_to_free = (header_t*)((unsigned char*)ptr - sizeof(header_t));
	header_cursor = header_start.next;

	if (!header_cursor || (header_t*)header_start.next > header_to_free) {
		header_to_free->next = header_start.next;
		header_start.next = header_to_free;
	} else {
		while (header_cursor->next && (header_t*)header_cursor->next <= header_to_free) {
			header_cursor = header_cursor->next;
		}

		// trying to free a free block
		if (header_cursor == header_to_free) {
			return;
		}

		header_to_free->next = header_cursor->next;
		header_cursor->next = header_to_free;
	}
}

// ------

void run_bench(void) {
#if THREAD_LOCAL_ALLOCATOR
	run_membench_thread_local(my_allocator_init, my_allocator_destroy, my_malloc, my_free);
#else
	run_membench_global(my_allocator_init, my_allocator_destroy, my_malloc, my_free);
#endif
}

void run_tests(void) {
	test_free_list_allocator();
}
