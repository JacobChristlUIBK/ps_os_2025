// This code can leak some memory but as far as i can tell it has to do with the queue implementation.

#include "pthread.h"
#include "bits/pthreadtypes.h"
#include "myqueue.h"
#include <assert.h>
#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include <time.h>

volatile sig_atomic_t hive_destroyed = 0;

typedef struct {
	bool** flowers;
	int width;
	int height;
} field_t;

field_t* field_init(int width, int height) {
	field_t* field = malloc(sizeof(*field));
	if (!field) {
		return NULL;
	}

	field->flowers = malloc(sizeof(*field->flowers) * width);
	if (!field->flowers) {
		free(field);
		return NULL;
	}

	for (int i = 0; i < width; i++) {
		field->flowers[i] = malloc(sizeof(**field->flowers) * height);
		if (!field->flowers[i]) {
			for (int j = 0; j < i; j++) {
				free(field->flowers[j]);
			}
			free(field->flowers);
			free(field);
			return NULL;
		}

		for (int j = 0; j < height; j++) {
			field->flowers[i][j] = false;
		}
	}

	field->width = width;
	field->height = height;

	return field;
}

void field_free(field_t* field) {
	if (!field) {
		return;
	}

	for (int i = 0; i < field->width; i++) {
		free(field->flowers[i]);
	}

	free(field->flowers);
	free(field);
}

void field_harvest(field_t* field, int x, int y) {
	if (field && x >= 0 && x < field->width && y >= 0 && y < field->height) {
		field->flowers[x][y] = true;
	}
}

int field_count_harvested(field_t* field) {
	int count = 0;

	for (int i = 0; i < field->width; i++) {
		for (int j = 0; j < field->height; j++) {
			if (field->flowers[i][j]) {
				count++;
			}
		}
	}

	return count;
}

bool field_is_harvested(field_t* field, int x, int y) {
	if (!field || x < 0 || x >= field->width || y < 0 || y >= field->height) {
		return true;
	} else {
		return field->flowers[x][y];
	}
}

bool field_all_is_harvested(field_t* field) {
	for (int i = 0; i < field->width; i++) {
		for (int j = 0; j < field->height; j++) {
			if (!field->flowers[i][j]) {
				return false;
			}
		}
	}

	return true;
}

pos_t field_random_pos(field_t* field) {
	return (pos_t){ .x = rand() % field->width, .y = rand() % field->height };
}

typedef struct {
	field_t* field;
	myqueue* queue;
	bool hive_destroyed;
	bool all_harvested;
	pthread_mutex_t field_mut;
	pthread_mutex_t queue_mut;
	pthread_barrier_t barrier_bear;
	pthread_barrier_t barrier_new_cycle;
} pl_t;

typedef struct bee {
	int id;
	pthread_t thread;
	struct bee* bees;
	int bees_size;
	pl_t* pl;
} bee_t;

enum bee_state {
	WORKING,
	COLLECTING,
};

void bee_sleep() {
	int amount_ms = rand() % 401 + 100;
	thrd_sleep(&(struct timespec){ .tv_nsec = amount_ms * 1000000 }, NULL);
}

void* bee_work(void* arg) {
	if (!arg) {
		pthread_exit(NULL);
		return NULL;
	}
	bee_t* bee = (bee_t*)arg;

	enum bee_state state = WORKING;
	pos_t position;

	pthread_mutex_lock(&bee->pl->field_mut);
	while (!bee->pl->all_harvested && !bee->pl->hive_destroyed) {
		pthread_mutex_unlock(&bee->pl->field_mut);

		if (state == WORKING) {
			pthread_mutex_lock(&bee->pl->queue_mut); // 130
			if (!myqueue_is_empty(bee->pl->queue)) {
				position = myqueue_pop(bee->pl->queue);
				state = COLLECTING;
				printf("Bee %d is flying to food source at position (%d,%d).\n", bee->id, position.x,
				       position.y);
			} else {
				printf("Bee %d is working in beehive.\n", bee->id);
			}
			pthread_mutex_unlock(&bee->pl->queue_mut);
		} else {
			pthread_mutex_lock(&bee->pl->field_mut);

			if (!field_is_harvested(bee->pl->field, position.x, position.y)) {
				field_harvest(bee->pl->field, position.x, position.y);
				printf("Bee %d collected nectar at position (%d,%d) and reports potential food sources: ",
				       bee->id, position.x, position.y);

				pos_t neighbours[4] = { { position.x + 1, position.y },
					                      { position.x - 1, position.y },
					                      { position.x, position.y + 1 },
					                      { position.x, position.y - 1 } };

				for (int i = 0; i < 4; i++) {
					if (!field_is_harvested(bee->pl->field, neighbours[i].x, neighbours[i].y)) {
						pthread_mutex_unlock(&bee->pl->field_mut);

						pthread_mutex_lock(&bee->pl->queue_mut);
						myqueue_push(bee->pl->queue, neighbours[i]);
						printf("(%d,%d)", neighbours[i].x, neighbours[i].y);
						pthread_mutex_unlock(&bee->pl->queue_mut);

						pthread_mutex_lock(&bee->pl->field_mut);
					}
				}

				printf(".\n");
			} else {
				printf("Bee %d could not find nectar at position (%d,%d).\n", bee->id, position.x,
				       position.y);
			}
			pthread_mutex_unlock(&bee->pl->field_mut);
			state = WORKING;
		}

		bee_sleep();

		int res = pthread_barrier_wait(&bee->pl->barrier_bear);
		if (res == PTHREAD_BARRIER_SERIAL_THREAD) {
			pthread_mutex_lock(&bee->pl->field_mut);
			bee->pl->all_harvested = field_all_is_harvested(bee->pl->field);
			pthread_mutex_unlock(&bee->pl->field_mut);

			if (rand() % 10 < 1) {
				printf("Bees encounter a bear and engage in a fight.\n");

				if (rand() % 2) {
					printf("Bear destroys the beehive.\n");
					pthread_mutex_lock(&bee->pl->field_mut);
					bee->pl->hive_destroyed = true;
					pthread_mutex_unlock(&bee->pl->field_mut);
				} else {
					printf("The bees successfully repel the bear and resume their work.\n");
				}
			}
		}
		pthread_barrier_wait(&bee->pl->barrier_new_cycle);

		pthread_mutex_lock(&bee->pl->field_mut);
	}
	pthread_mutex_unlock(&bee->pl->field_mut);

	pthread_exit(NULL);
	return NULL;
}

int main(int argc, char* argv[]) {
	if (argc != 4) {
		printf("usage: %s <flower field width> <flower field height> <number of bees>\n", argv[0]);
		return EXIT_FAILURE;
	}

	int width, height, bee_amount;

	width = atoi(argv[1]);
	height = atoi(argv[2]);
	bee_amount = atoi(argv[3]);

	if (width < 1 || height < 1) {
		fprintf(stderr, "error: field must be at least 1x1");
		return EXIT_FAILURE;
	}

	if (bee_amount < 1) {
		fprintf(stderr, "error: at least 1 bee is required");
		return EXIT_FAILURE;
	}

	pl_t* pl = malloc(sizeof(*pl));
	if (!pl) {
		perror("malloc");
		return EXIT_FAILURE;
	}

	pl->field = field_init(width, height);
	if (!pl->field) {
		perror("field_init");
		return EXIT_FAILURE;
	}

	myqueue queue;
	myqueue_init(&queue);
	pl->queue = &queue;

	pl->hive_destroyed = false;
	pl->all_harvested = false;

	if (pthread_mutex_init(&pl->queue_mut, NULL)) {
		perror("mutex_init");
		return EXIT_FAILURE;
	}

	if (pthread_mutex_init(&pl->field_mut, NULL)) {
		perror("mutex_init");
		return EXIT_FAILURE;
	}

	if (pthread_barrier_init(&pl->barrier_bear, NULL, bee_amount)) {
		perror("barrier_init");
		return EXIT_FAILURE;
	}

	if (pthread_barrier_init(&pl->barrier_new_cycle, NULL, bee_amount)) {
		perror("barrier_init");
		return EXIT_FAILURE;
	}

	myqueue_push(pl->queue, field_random_pos(pl->field));

	bee_t bees[bee_amount];

	for (int i = 0; i < bee_amount; i++) {
		bees[i].id = i;
		bees[i].pl = pl;
		bees[i].bees = (bee_t*)&bees;
		bees[i].bees_size = bee_amount;

		if (pthread_create(&bees[i].thread, NULL, bee_work, &bees[i])) {
			perror("pthread_create");
			return EXIT_FAILURE; // TODO
		}
	}

	for (int i = 0; i < bee_amount; i++) {
		if (pthread_join(bees[i].thread, NULL)) {
			perror("pthread_create");
			return EXIT_FAILURE; // TODO
		}
	}

	printf("%d bees collected nectar from %d/%d flowers.\n", bee_amount,
	       field_count_harvested(pl->field), width * height);
	printf("Beehive was%s destroyed.\n", pl->hive_destroyed ? "" : " not");

	pthread_barrier_destroy(&pl->barrier_bear);
	pthread_barrier_destroy(&pl->barrier_new_cycle);
	pthread_mutex_destroy(&pl->field_mut);
	pthread_mutex_destroy(&pl->queue_mut);
	field_free(pl->field);
	free(pl);
}
