#ifndef MY_PTHREAD_BARRIER_H
#define MY_PTHREAD_BARRIER_H

#include <pthread.h>

#if defined(__GNUC__) || defined(__clang__)
    #define UNUSED_PARAM __attribute__((unused))
#elif defined(_MSC_VER)
    #define UNUSED_PARAM __pragma(warning(suppress: 4100 4101 4505))
#else
    #define UNUSED_PARAM
#endif


typedef struct {
    // TODO: implement me!
} my_pthread_barrier_t;

int my_pthread_barrier_init(my_pthread_barrier_t *barrier, UNUSED_PARAM void* attr, int count);
int my_pthread_barrier_wait(my_pthread_barrier_t *barrier);
int my_pthread_barrier_destroy(my_pthread_barrier_t *barrier);

#endif // MY_PTHREAD_BARRIER_H