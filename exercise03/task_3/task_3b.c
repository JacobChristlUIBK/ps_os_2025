#include "pthread.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_BUF_SIZE 32

typedef struct {
	pthread_t pt;
	int id;
	long long sum;
	// assumes paths to be not longer than 256 characters
	char path[256];
} sum_thread_t;

// long long sum_file(const char* path) {
void* sum_file(void* sum_thr) {
	if (!sum_thr) {
		return NULL;
	}

	sum_thread_t* thread = (sum_thread_t*)sum_thr;

	FILE* file = fopen(thread->path, "r");
	if (!file) {
		return NULL;
	}

	thread->sum = 0;
	char line_buf[LINE_BUF_SIZE];
	line_buf[0] = '\0';

	while (fgets(line_buf, LINE_BUF_SIZE, file)) {
		thread->sum += (long long)atoi(line_buf);
	}

	return thread;
}

int main(int argc, char* argv[]) {
	long long total_sum = 0;
	sum_thread_t** threads = malloc(sizeof(*threads) * (argc - 1));

	for (int i = 0; i < argc - 1; i++) {
		threads[i] = malloc(sizeof(**threads));

		threads[i]->id = i;
		threads[i]->sum = 0;
		strcpy(threads[i]->path, argv[i + 1]);

		if (pthread_create(&threads[i]->pt, NULL, sum_file, threads[i]) < 0) {
			fprintf(stderr, "failed to create thread\n");
			return EXIT_FAILURE;
		}
	}

	for (int i = 0; i < argc - 1; i++) {
		if (pthread_join(threads[i]->pt, NULL) < 0) {
			fprintf(stderr, "failed to join thread with id %d\n", threads[i]->id);
		}

		printf("sum %d = %lld\n", threads[i]->id, threads[i]->sum);
		total_sum += threads[i]->sum;
	}

	printf("total sum = %lld\n", total_sum);
	free(threads);

	return EXIT_SUCCESS;
}
