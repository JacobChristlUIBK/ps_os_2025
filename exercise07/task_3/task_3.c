#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "slot_machine.h"

#if ((!defined(THREAD_PER_JOB) && !defined(THREAD_POOL)) || (defined(THREAD_PER_JOB) && defined(THREAD_POOL)))
	#error "Define exactly one of THREAD_PER_JOB, THREAD_POOL" // this causes an error
#endif

#ifdef THREAD_POOL
	#include "thread_pool.h"
	#define POOL_SIZE 5
#else
	#include <pthread.h>
#endif

#define NUM_JOBS 2000
#define NUM_TRIES 100000lu

//#define PRINT_SPINS 1

typedef struct job_struct {
	unsigned int seed;
	uint64_t result;
	SlotMachine* slot_machine;
} job_struct;

// Simulate that all reels on a slot machine produce the same symbol
void* simulate_all_symbols_equal(void* arg) {
	job_struct* job = (job_struct*)arg;
	for (long unsigned i = 0; i < NUM_TRIES; i++) {
		// we spin every reel once => num reels = num spins
		int num_reels = job->slot_machine->num_reels;
		SlotSymbol first_spin_result;
		bool are_all_spins_equal = true;

#ifdef PRINT_SPINS
			SlotSymbol spins[num_reels];
#endif

		for (int j = 0; j < num_reels; j++) {
			Reel* reel = job->slot_machine->reels[j];
			int random_number = rand_r(&job->seed);
			SlotSymbol spin_result = get_random_spin_result(reel, random_number);

#ifdef PRINT_SPINS			
				spins[j] = spin_result;
#endif	

			if (j == 0) {
				first_spin_result = spin_result;
			} else {
				if (!are_spin_results_equal(spin_result, first_spin_result)) {
					are_all_spins_equal = false;
#ifndef PRINT_SPINS	
						break;
#endif
				}
			}
		}

		// Try was successful if the symbols on all reels are equal
		if (are_all_spins_equal) {
			(job->result)++;
		}

#ifdef PRINT_SPINS
		printf("\r| %s | %s | %s | 🎰 (# same symbol on all reels: %ld)", spins[0], spins[1], spins[2], job->result);
		fflush(stdout); 
#endif
	}

	return NULL;
}

int main(void) {
	job_struct* jobs = malloc(NUM_JOBS * sizeof(job_struct));
	if (jobs == NULL) {
		fprintf(stderr, "Could not allocate memory for jobs.\n");
		exit(EXIT_FAILURE);
	}
	
	SlotMachine* slot_machine = create_slot_machine();

#ifdef THREAD_POOL
	thread_pool my_pool;
	pool_create(&my_pool, POOL_SIZE);
	printf("Creating thread pool with size %d.\n", POOL_SIZE);

	job_id job_ids[NUM_JOBS];
#else
	pthread_t threads[NUM_JOBS];
#endif
	for (size_t i = 0; i < NUM_JOBS; i++) {
		jobs[i] = (job_struct){ .seed = i, .result = 0, .slot_machine=slot_machine };
#ifdef THREAD_POOL
		job_ids[i] = pool_submit(&my_pool, simulate_all_symbols_equal, jobs + i);
#else
		if (pthread_create(&threads[i], NULL, simulate_all_symbols_equal, jobs + i) != 0) {
			perror("Creating Threads");
			return EXIT_FAILURE;
		}
#endif
	}

	uint64_t min_result = UINT64_MAX;
	uint64_t max_result = 0;
	for (size_t i = 0; i < NUM_JOBS; i++) {
#ifdef THREAD_POOL
		pool_await(job_ids[i]);
#else
		if (pthread_join(threads[i], NULL) != 0) {
			perror("Joining Threads");
			return EXIT_FAILURE;
		}
#endif
		unsigned job_result = jobs[i].result;
		min_result = job_result < min_result ? job_result : min_result;
		max_result = job_result > max_result ? job_result : max_result;
	}

#ifdef THREAD_POOL
	pool_destroy(&my_pool);
#endif
	free(jobs);
	double probability = get_all_equal_probability(slot_machine);
	destroy_slot_machine(slot_machine);

#ifdef PRINT_SPINS
		printf("\n");
#endif
	printf("The percentage occurences of the same symbol on all reels was maximum %.2f%% and a minimum %.2f%%, of expected %.2f%%.\n", (float) max_result / NUM_TRIES * 100, (float) min_result / NUM_TRIES * 100, probability * 100);
}
