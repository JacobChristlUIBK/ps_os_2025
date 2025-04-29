#include "myqueue.h"
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// so each thread has a dedicated sum
typedef struct {
	int id;
	int sum;
	pthread_t thread;
} consumer_t;

// initializes a static mutex with default attributes
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
myqueue queue;

void* consumer(void* arg) {
	if (!arg) {
		pthread_exit(NULL);
	}

	consumer_t* consumer = (consumer_t*)arg;
	consumer->sum = 0;

	while (1) {
		pthread_mutex_lock(&queue_mutex);
		if (myqueue_is_empty(&queue)) {
			pthread_mutex_unlock(&queue_mutex);
			continue;
		}

		int value = myqueue_pop(&queue);
		pthread_mutex_unlock(&queue_mutex);

		if (value == INT_MAX) {
			printf("Consumer %d sum: %d\n", consumer->id, consumer->sum);
			pthread_exit(NULL);
		}

		consumer->sum += value;
	}
}

int main(int argc, char* argv[]) {
	if (argc != 3) {
		fprintf(stderr, "usage: %s <number_of_consumers> <number_of_elements>\n", argv[0]);
		return EXIT_FAILURE;
	}

	int c = atoi(argv[1]);
	int n = atoi(argv[2]);

	if (c <= 0 || n <= 0) {
		fprintf(stderr, "error: both arguments must be positive integers.\n");
		return EXIT_FAILURE;
	}

	consumer_t* consumers = malloc(c * sizeof(consumer_t));
	if (!consumers) {
		perror("failed to allocate memory for consumers");
		return EXIT_FAILURE;
	}

	for (int i = 0; i < c; i++) {
		consumers[i].id = i;

		if (pthread_create(&consumers[i].thread, NULL, consumer, &consumers[i]) != 0) {
			perror("failed to create consumer thread");
			free(consumers);
			return EXIT_FAILURE;
		}
	}

	myqueue_init(&queue);

	for (int i = 1; i <= n; i++) {
		pthread_mutex_lock(&queue_mutex);
		myqueue_push(&queue, i % 2 ? i : -i);
		pthread_mutex_unlock(&queue_mutex);
	}

	for (int i = 0; i < c; i++) {
		pthread_mutex_lock(&queue_mutex);
		myqueue_push(&queue, INT_MAX);
		pthread_mutex_unlock(&queue_mutex);
	}

	int final_sum = 0;

	for (int i = 0; i < c; i++) {
		if (pthread_join(consumers[i].thread, NULL) != 0) {
			perror("Failed to join consumer thread");
			free(consumers);
			return EXIT_FAILURE;
		}

		final_sum += consumers[i].sum;
	}

	printf("final sum: %d\n", final_sum);

	free(consumers);
	pthread_mutex_destroy(&queue_mutex);

	return EXIT_SUCCESS;
}
