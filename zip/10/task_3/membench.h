#ifndef MEMBENCH_H
#define MEMBENCH_H

#include <stddef.h>

typedef void (*init_allocator_fn)(size_t);
typedef void (*destroy_allocator_fn)(void);

typedef void* (*malloc_fn)(size_t);
typedef void (*free_fn)(void*);

void run_membench_global(init_allocator_fn my_init, destroy_allocator_fn my_destroy,
                         malloc_fn my_malloc, free_fn my_free);

void run_membench_thread_local(init_allocator_fn my_init, destroy_allocator_fn my_destroy,
                               malloc_fn my_malloc, free_fn my_free);

#endif
