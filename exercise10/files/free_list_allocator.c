#include <assert.h>
#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include "allocator.h"
#include "allocator_tests.h"
#include "membench.h"

// TODO: Changes might be needed 

void my_allocator_init(size_t total_size) {
	#error "TODO: Implement my_allocator_init"
}

void my_allocator_destroy(void) {
	#error "TODO: Implement my_allocator_destroy"
}

void* my_malloc(size_t size) {
	#error "TODO: Implement my_malloc"
}

void my_free(void* ptr) {
	#error "TODO: Implement my_free"
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
