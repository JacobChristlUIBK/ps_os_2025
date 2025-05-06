#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 500

int counter = 0;
pthread_mutex_t mutex;

void* thread_function(void* arg) {
	for (int i = 0; i < 50000; i++) {
		pthread_mutex_lock(&mutex);
		if (i % 2 == 0) {
			counter += 73;
		} else {
			counter -= 71;
		}
		pthread_mutex_unlock(&mutex);
	}

	// return arg to avoid unused parameter errors
	return arg;
}

int main() {
	pthread_t threads[NUM_THREADS];
	pthread_mutex_init(&mutex, NULL); // NULL equates to default attributes

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

	pthread_mutex_destroy(&mutex);
	return EXIT_FAILURE;
}
