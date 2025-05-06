#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define EAT_DURATION 100
#define NUM_PHILOSOPHER 15
#define EAT_ITERATIONS 10000
#define RIGHT_CHOPSTICK(philosopher_id) (philosopher_id)
#define LEFT_CHOPSTICK(philosopher_id) (((philosopher_id) + 1) % NUM_PHILOSOPHER)

typedef struct {
	int id;
	pthread_mutex_t* chopstick;
} philosopher_t;

void* dine(void* arg) {
	philosopher_t philosopher_parameters = *(philosopher_t*)arg;
	for (int i = 0; i < EAT_ITERATIONS; ++i) {
		pthread_mutex_lock(&philosopher_parameters.chopstick[RIGHT_CHOPSTICK(philosopher_parameters.id)]);
		pthread_mutex_lock(&philosopher_parameters.chopstick[LEFT_CHOPSTICK(philosopher_parameters.id)]);
		usleep(EAT_DURATION);
		pthread_mutex_unlock(&philosopher_parameters.chopstick[LEFT_CHOPSTICK(philosopher_parameters.id)]);
		pthread_mutex_unlock(&philosopher_parameters.chopstick[RIGHT_CHOPSTICK(philosopher_parameters.id)]);
	}
	printf("Philosopher %d is done eating!\n", philosopher_parameters.id);

	return NULL;
}

int main(void) {
	pthread_t philosopher[NUM_PHILOSOPHER];
	pthread_mutex_t chopstick[NUM_PHILOSOPHER];
	philosopher_t philosopher_parameters[NUM_PHILOSOPHER];

	for (int i = 0; i < NUM_PHILOSOPHER; ++i) {
		philosopher_parameters[i].id = i;
		philosopher_parameters[i].chopstick = chopstick;
		pthread_mutex_init(&chopstick[i], NULL);
	}

	for (int i = 0; i < NUM_PHILOSOPHER; ++i) {
		pthread_create(&philosopher[i], NULL, dine, (void*) (philosopher_parameters + i));
	}

	for (int i = 0; i < NUM_PHILOSOPHER; ++i) {
		pthread_join(philosopher[i], NULL);
	}

	for (int i = 0; i < NUM_PHILOSOPHER; ++i) {
		pthread_mutex_destroy(&chopstick[i]);
	}

	return EXIT_SUCCESS;
}
