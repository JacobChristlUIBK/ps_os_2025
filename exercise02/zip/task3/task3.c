#include "bits/time.h"
#include "sys/types.h"
#include "sys/wait.h"
#include <time.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// https://kappa.lol/lFt_s_
double DR_p(const int t, const int n, const unsigned long long s) {
	unsigned long long h = 0;
	for (unsigned long long i = s; i--;) {
		h += (rand() % n + rand() % n + 2 == t);
	}
	return (double)h / s;
}

// Simulates for a single target number.
// Returns -1 if timer failed to measure time.
// Otherwise returns 0.
int simulate_target(int target, int eyes, int rolls, struct timespec *timer_start) {
	// execute children
	int child_num = target - 2;

	// seed with pid
	srand(getpid());
	double odds = DR_p(target, eyes, rolls);

	// end timer
	struct timespec timer_end;
	if (clock_gettime(CLOCK_REALTIME, &timer_end) == -1) {
		return -1;
	};

	// timer diff in seconds
	double diff = (timer_end.tv_sec - timer_start->tv_sec) + (timer_end.tv_nsec - timer_start->tv_nsec) / 1000000000.0;

	printf(
		"Child %d PID = %d. DR_p(%d,%d,%d>) = %f. Elapsed time = %.6fs\n",
		child_num,
		getpid(),
		target,
		eyes,
		rolls,
		odds,
		diff
	);

	return 0;
}

// Simulates for all targets.
// Returns -1 if some timer failed to measure time.
// Returns -2 if forking failed.
// Returns 0 otherwise.
int simulate(int eyes, int rolls, struct timespec *timer_start) {
	pid_t pid = 1;	// set pid to something != 0
	int target = 1;	// target can be set to 1 since it will be increased by one after forking

	// create forks for each target number
	// this loop stops in child processes
	while (pid && target < eyes * 2) {
		pid = fork();
		target++;

		if (pid == -2) {
			fprintf(stderr, "error: failed to fork for target %d", target);
			return -2;
		}
	}

	if (pid) {
		// collect children
		for (int i = 2; i <= eyes * 2; i++) {
			wait(NULL);
		}

		printf("Done.\n");
	} else {
		if (simulate_target(target, eyes, rolls, timer_start) == -1) {
			fprintf(stderr, "error: failed to start timer");
			return -1;
		}

		exit(EXIT_SUCCESS);
	}

	return 0;
}

int main(int argc, char *argv[]) {
	// start timer as soon as possible
	struct timespec timer_start;
	if (clock_gettime(CLOCK_REALTIME, &timer_start) == -1) {
		fprintf(stderr, "error: failed to start timer");
		return EXIT_FAILURE;
	}

	// check arg amount
	if (argc != 3) {
		fprintf(stderr, "usage: ./task3 <sides> <throws>");
		return EXIT_FAILURE;
	}

	// check if user entered valid amount of sides
	int eyes = atoi(argv[1]);
	if (eyes < 1) {
		fprintf(stderr, "error: dice have to have at least 1 side");
		return EXIT_FAILURE;
	}

	// check if user entered valid amount of simulation rolls
	int rolls = atoi(argv[2]);
	if (rolls < 1) {
		fprintf(stderr, "error: have to roll at least 1 time");
		return EXIT_FAILURE;
	}

	// process the simulation
	if (simulate(eyes, rolls, &timer_start) != EXIT_SUCCESS) {
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
