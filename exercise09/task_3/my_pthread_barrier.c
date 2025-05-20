#include "my_pthread_barrier.h"
#include "pthread.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

int my_pthread_barrier_init(my_pthread_barrier_t* barrier, UNUSED_PARAM void* attr, int count) {
	if (!barrier) {
		return -1;
	}

	if (count < 1) {
		return -4;
	}

	int res;

	barrier->waiting_threads = 0;
	barrier->max_threads = count;
	barrier->generation = 0;

	res = pthread_mutex_init(&barrier->barrier_mutex, NULL);
	if (res < 0) {
		return -2;
	}

	res = pthread_cond_init(&barrier->waiting_full_cond, NULL);
	if (res < 0) {
		pthread_mutex_destroy(&barrier->barrier_mutex);
		return -3;
	}

	res = pthread_cond_init(&barrier->next_gen_cond, NULL);
	if (res < 0) {
		pthread_mutex_destroy(&barrier->barrier_mutex);
		pthread_cond_destroy(&barrier->waiting_full_cond);
		return -3;
	}

	return 0;
}

int my_pthread_barrier_wait(my_pthread_barrier_t* barrier) {
	if (!barrier) {
		return -1;
	}

	pthread_mutex_lock(&barrier->barrier_mutex);

	int generation = barrier->generation;
	barrier->waiting_threads++;

	if (barrier->waiting_threads < barrier->max_threads) {
		while (generation == barrier->generation && barrier->waiting_threads < barrier->max_threads) {
			pthread_cond_wait(&barrier->waiting_full_cond, &barrier->barrier_mutex);
		}
	} else {
		barrier->waiting_threads = 0;
		barrier->generation++;
		pthread_cond_broadcast(&barrier->waiting_full_cond);
		pthread_mutex_unlock(&barrier->barrier_mutex);

		return MY_PTHREAD_BARRIER_SERIAL_THREAD;
	}

	pthread_mutex_unlock(&barrier->barrier_mutex);
	return 0;
}

int my_pthread_barrier_destroy(my_pthread_barrier_t* barrier) {
	if (!barrier) {
		return -1;
	}

	pthread_mutex_lock(&barrier->barrier_mutex);
	if (barrier->waiting_threads > 0) {
		pthread_mutex_unlock(&barrier->barrier_mutex);
		return -2;
	}

	pthread_mutex_unlock(&barrier->barrier_mutex);
	pthread_cond_destroy(&barrier->waiting_full_cond);
	pthread_mutex_destroy(&barrier->barrier_mutex);
	return 0;
}
