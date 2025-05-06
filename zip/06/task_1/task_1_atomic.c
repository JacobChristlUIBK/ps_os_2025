#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 500

atomic_int counter = 0;

void* thread_function(void* arg) {
	for (int i = 0; i < 50000; i++) {
		if (i % 2 == 0) {
			atomic_fetch_add(&counter, 73);
		} else {
			atomic_fetch_sub(&counter, 71);
		}
	}

	// return arg to avoid unused parameter errors
	return arg;
}

int main() {
	pthread_t threads[NUM_THREADS];

	for (int i = 0; i < NUM_THREADS; i++) {
		if (pthread_create(&threads[i], NULL, thread_function, NULL) != 0) {
			perror("failed to create thread");
			return EXIT_FAILURE;
		}
	}

	for (int i = 0; i < NUM_THREADS; i++) {
		if (pthread_join(threads[i], NULL) != 0) {
			perror("failed to join thread");
			return EXIT_FAILURE;
		}
	}

	printf("%d\n", counter);
	return EXIT_SUCCESS;
}
