#include "pthread.h"
#include "bits/pthreadtypes.h"
#include "myqueue.h"
#include "sched.h"
#include <stdio.h>
#include <string.h>
#include <stdatomic.h>
#include <stddef.h>
#include <stdlib.h>

// https://en.cppreference.com/w/c/atomic/atomic_flag
// atomic flags seem way more straight forward

typedef struct {
	// use this struct to signal the user to not modify fields
	struct {
		volatile atomic_flag locked;
	} PRIVATE;
} c_mutex_t;

int c_mutex_init(c_mutex_t* mutex, void* _) {
	_ = _;	// avoid unused parameter warning

	atomic_flag a = ATOMIC_FLAG_INIT;
	mutex->PRIVATE.locked = a;
	return 0;
}

int c_mutex_lock(c_mutex_t* mutex) {
	while (atomic_flag_test_and_set(&mutex->PRIVATE.locked)) {
		sched_yield();
	}
	return 0;
}

int c_mutex_unlock(c_mutex_t* mutex) {
	atomic_flag_clear(&mutex->PRIVATE.locked);
	return 0;
}

/* defines */

#if CUSTOM_MUTEX
	#define mutex_t c_mutex_t
	#define MUTEX_INIT(x) c_mutex_init(x, NULL)
	#define MUTEX_LOCK(x) c_mutex_lock(x)
	#define MUTEX_UNLOCK(x) c_mutex_unlock(:fo:x)
#else
	#define mutex_t pthread_mutex_t
	#define MUTEX_INIT(x) pthread_mutex_init(x, NULL)
	#define MUTEX_LOCK(x) pthread_mutex_lock(x)
	#define MUTEX_UNLOCK(x) pthread_mutex_unlock(x)
#endif

/* main code */

typedef struct {
	myqueue queue;
	mutex_t queue_mut;
} pl_t;

void* populate(void* pl_) {
	if (!pl_) {
		pthread_exit(NULL);
	}
	pl_t* pl = (pl_t*)pl_;

	for (size_t i = 0; i < 10000000; i++) {
		MUTEX_LOCK(&pl->queue_mut);
		myqueue_push(&pl->queue, 1);
		MUTEX_UNLOCK(&pl->queue_mut);
	}

	MUTEX_LOCK(&pl->queue_mut);
	myqueue_push(&pl->queue, 0);
	MUTEX_UNLOCK(&pl->queue_mut);

	pthread_exit(NULL);
}

void* sum(void* pl_) {
	if (!pl_) {
		pthread_exit(NULL);
	}
	pl_t* pl = (pl_t*)pl_;

	int sum = 0;

	while (1) {
		MUTEX_LOCK(&pl->queue_mut);
		if (myqueue_is_empty(&pl->queue)) {
			MUTEX_UNLOCK(&pl->queue_mut);
			continue;
		}

		int buf = myqueue_pop(&pl->queue);
		MUTEX_UNLOCK(&pl->queue_mut);

		if (buf == 0) {
			break;
		}

		sum += buf;
	}

	printf("overall sum: %d\n", sum);

	pthread_exit(NULL);
}

int main() {
	pl_t pl;

	myqueue_init(&pl.queue);

	MUTEX_INIT(&pl.queue_mut);

	pthread_t populate_thread;
	pthread_t sum_thread;

	pthread_create(&populate_thread, NULL, populate, &pl);
	pthread_create(&sum_thread, NULL, sum, &pl);

	pthread_join(populate_thread, NULL);
	pthread_join(sum_thread, NULL);

	return EXIT_SUCCESS;
}
