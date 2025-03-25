#include "bits/pthreadtypes.h"
#include "pthread.h"
#include "sys/wait.h"
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int64_t accumulation = 0;

void* accumulate(void* val) {
	if (!val) {
		return NULL;
	}

	int n = *(int*)val;

	for (int i = 0; i < n; i++) {
		accumulation += i;
	}

	pthread_exit(EXIT_SUCCESS);
	return NULL;
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		fprintf(stderr, "usage: %s <int n>", argv[0]);
		return EXIT_FAILURE;
	}

	int n = atoi(argv[1]);
	if (n < 1) {
		fprintf(stderr, "n has to be a positive integer");
		return EXIT_FAILURE;
	}

	// PRIu64 from inttypes.h for printing of int64_t accross different targets
	printf("Start: %" PRIu64 "\n", accumulation);

	pid_t pid = fork();

	if (pid == -1) {
		perror("couldn't fork");
		return EXIT_FAILURE;
	} else if (pid == 0) {
		for (int i = 0; i < n; i++) {
			accumulation += i;
		}
		exit(EXIT_SUCCESS);
	} else {
		wait(NULL);
		printf("After fork: %" PRIu64 "\n", accumulation);
	}

	pthread_t pt;

	if (pthread_create(&pt, NULL, &accumulate, &n) < 0) {
		fprintf(stderr, "failed to create thread");
	}

	if (pthread_join(pt, NULL) < 0) {
		fprintf(stderr, "failed to join thread");
	}

	printf("After thread: %" PRIu64 "\n", accumulation);
	return EXIT_SUCCESS;
}

/*
First print prints 0 as expected.

The print after the fork also prints 0.
This is because the forked process recieves it's complete own memory copied over from the parent
process including global values. The value of `accumulation` gets copied over, increased *inside*
the forked process. Once that process ends and the value is printed back in the parent process it is
still 0, since the accumulation only affected the copied memory.

The last print, actually prints the expected valued of 499500, since the thread shares memory with
the main thread, except for it's stack.
*/
