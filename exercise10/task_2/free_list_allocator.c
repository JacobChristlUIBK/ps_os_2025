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
	void* next_free;
} header_t;

static void* pool = NULL;
static size_t pool_size = 0;
static header_t header_start = { .next_free = NULL };
static pthread_mutex_t free_list_alloc_mutex = PTHREAD_MUTEX_INITIALIZER;
static int ops = 0;

// initializes a new pool
void my_allocator_init(size_t total_size) {
	int block_count = 0;
	size_t true_size = total_size - (total_size % HEADER_BLOCK_SIZE);
	size_t remaining_space = true_size;

	pthread_mutex_lock(&free_list_alloc_mutex);

	// return if pool has already been initialized
	if (pool != NULL) {
		pthread_mutex_unlock(&free_list_alloc_mutex);
		return;
	}

	// map pool
	pool = mmap(NULL, true_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (pool == MAP_FAILED) {
		pool = NULL;
		pthread_mutex_unlock(&free_list_alloc_mutex);
		return;
	}

	header_start.next_free = pool;
	header_t* cursor = &header_start;

	// populate pool with headers
	while (cursor) {
		// if this is the last block to be initialized, point to next header, otherwise NULL
		cursor->next_free = remaining_space / HEADER_BLOCK_SIZE > 1
		                        ? (void*)((unsigned long)pool + (HEADER_BLOCK_SIZE * (block_count + 1)))
		                        : NULL;

		cursor = cursor->next_free;

		block_count++;
		remaining_space -= HEADER_BLOCK_SIZE;
	}

	pool_size = true_size;

	pthread_mutex_unlock(&free_list_alloc_mutex);
}

void my_allocator_destroy(void) {
	pthread_mutex_lock(&free_list_alloc_mutex);

	if (!pool) {
		pthread_mutex_unlock(&free_list_alloc_mutex);
		return;
	}

	int res;

	res = munmap(pool, pool_size);
	if (res == -1) {
		pthread_mutex_unlock(&free_list_alloc_mutex);
		return;
	}

	pool = NULL;
	pool_size = 0;
	header_start.next_free = NULL;

	pthread_mutex_unlock(&free_list_alloc_mutex);
}

void* my_malloc(size_t size) {
	header_t* block = NULL;

	pthread_mutex_lock(&free_list_alloc_mutex);

	if (!pool) {
		pthread_mutex_unlock(&free_list_alloc_mutex);
		return NULL;
	}

	if (size > BLOCK_SIZE) {
		pthread_mutex_unlock(&free_list_alloc_mutex);
		return NULL;
	}

	if (!header_start.next_free) {
		pthread_mutex_unlock(&free_list_alloc_mutex);
		return NULL;
	}

	block = header_start.next_free;
	header_start.next_free = block->next_free;

	pthread_mutex_unlock(&free_list_alloc_mutex);

	return (unsigned char*)block + sizeof(header_t);
}

void my_free(void* ptr) {
	header_t* header_to_free = NULL;
	header_t* header_cursor = NULL;

	pthread_mutex_lock(&free_list_alloc_mutex);

	if (!pool) {
		pthread_mutex_unlock(&free_list_alloc_mutex);
		return;
	}

	header_to_free = (header_t*)((unsigned char*)ptr - sizeof(header_t));
	header_cursor = header_start.next_free;

	if (!header_cursor || (header_t*)header_start.next_free > header_to_free) {
		header_to_free->next_free = header_start.next_free;
		header_start.next_free = header_to_free;
	} else {
		while (header_cursor->next_free && (header_t*)header_cursor->next_free <= header_to_free) {
			header_cursor = header_cursor->next_free;
		}

		// trying to free a free block
		if (header_cursor == header_to_free) {
			return;
		}

		header_to_free->next_free = header_cursor->next_free;
		header_cursor->next_free = header_to_free;
	}

	pthread_mutex_unlock(&free_list_alloc_mutex);
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
