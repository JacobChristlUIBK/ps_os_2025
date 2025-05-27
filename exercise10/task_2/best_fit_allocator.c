#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include "allocator.h"
#include "allocator_tests.h"
#include "membench.h"

// Holds the header of a block, pointing to the next free block.
typedef struct header {
	bool is_free;
	size_t size;
	struct header* next;
	struct header* prev;
} header_t;

static void* pool = NULL;
static size_t pool_size = 0;
static header_t header_start = { .next = NULL, .prev = NULL, .size = 0, .is_free = false };
static pthread_mutex_t free_list_alloc_mutex = PTHREAD_MUTEX_INITIALIZER;

// initializes a new pool
void my_allocator_init(size_t total_size) {
	pthread_mutex_lock(&free_list_alloc_mutex);

	// return if pool has already been initialized
	if (pool != NULL) {
		pthread_mutex_unlock(&free_list_alloc_mutex);
		return;
	}

	// map pool
	pool = mmap(NULL, total_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (pool == MAP_FAILED) {
		pool = NULL;
		pthread_mutex_unlock(&free_list_alloc_mutex);
		return;
	}

	header_start.next = (header_t*)pool;
	header_start.prev = NULL;
	header_start.size = 0;
	header_start.is_free = false;

	header_start.next->next = NULL;
	header_start.next->prev = NULL;
	header_start.next->size = total_size;
	header_start.next->is_free = true;
	pool_size = total_size;

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
	header_start.next = NULL;

	pthread_mutex_unlock(&free_list_alloc_mutex);
}

void* my_malloc(size_t size) {
	size_t block_size = size + sizeof(header_t);
	size_t offset = 0;

	pthread_mutex_lock(&free_list_alloc_mutex);

	if (!pool || size <= 0) {
		pthread_mutex_unlock(&free_list_alloc_mutex);
		return NULL;
	}

	if (!header_start.next) {
		pthread_mutex_unlock(&free_list_alloc_mutex);
		return NULL;
	}

	header_t* cursor = header_start.next;

	while (cursor) {
		if (cursor->is_free && cursor->size >= block_size) {
			break;
		}

		offset += cursor->size;
		cursor = cursor->next;
	}

	if (!cursor) {
		pthread_mutex_unlock(&free_list_alloc_mutex);
		return NULL;
	}

	if (cursor->size < block_size + sizeof(header_t)) {
		block_size = cursor->size;
	} else {
		header_t* next_buf = cursor->next;

		cursor->next = (void*)((unsigned char*)cursor + block_size);
		cursor->next->next = next_buf;
		cursor->next->prev = cursor;
		cursor->next->is_free = true;
		cursor->next->size = cursor->size - block_size;
	}

	cursor->is_free = false;
	cursor->size = block_size;

	pthread_mutex_unlock(&free_list_alloc_mutex);

	return (unsigned char*)cursor + sizeof(header_t);
}

void my_free(void* ptr) {
	header_t* header_to_free = NULL;
	header_t* cursor = NULL;

	pthread_mutex_lock(&free_list_alloc_mutex);

	if (!pool) {
		pthread_mutex_unlock(&free_list_alloc_mutex);
		return;
	}

	header_to_free = (header_t*)((unsigned char*)ptr - sizeof(header_t));

	if (header_to_free->is_free) {
		pthread_mutex_unlock(&free_list_alloc_mutex);
		return;
	}

	header_to_free->is_free = true;

	int merge_size = header_to_free->size;
	header_t* next_buf = NULL;

	cursor = header_to_free;
	while (cursor->next && cursor->next->is_free) {
		merge_size += cursor->next->size;
		cursor = cursor->next;
	}

	next_buf = cursor->next;

	cursor = header_to_free;
	while (cursor->prev && cursor->prev->is_free) {
		merge_size += cursor->prev->size;
		cursor = cursor->prev;
	}

	cursor->next = next_buf;
	if (cursor->next) {
		next_buf->prev = cursor;
	}
	cursor->size = merge_size;

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
	test_best_fit_allocator();
}
