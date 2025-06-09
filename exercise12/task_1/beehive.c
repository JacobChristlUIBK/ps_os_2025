#include "pthread.h"
#include "bits/pthreadtypes.h"
#include "myqueue.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include <time.h>

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

typedef struct {
	field_t* field;
	myqueue* queue;
	pthread_mutex_t field_mut;
	pthread_mutex_t queue_mut;
} pl_t;

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
	pl_t *pl = (pl_t *)arg;

	enum bee_state state = WORKING;
	pos_t position;

	pthread_mutex_lock(&pl->field_mut);
	while (!field_all_is_harvested(pl->field)) {
		pthread_mutex_unlock(&pl->field_mut);

		if (state == WORKING) {
			pthread_mutex_lock(&pl->queue_mut);	// 130
			if (!myqueue_is_empty(pl->queue)) {
				position = myqueue_pop(pl->queue);
				state = COLLECTING;
			}
			pthread_mutex_unlock(&pl->queue_mut);
		} else {
			pthread_mutex_lock(&pl->field_mut);

			if (!field_is_harvested(pl->field, position.x, position.y)) {
				field_harvest(pl->field, position.x, position.y);

				pos_t neighbours[4] = {
					{position.x+1, position.y},
					{position.x-1, position.y},
					{position.x, position.y+1},
					{position.x, position.y-1}
				};

				for (int i = 0; i < 4; i++) {
					if (!field_is_harvested(pl->field, neighbours[i].x, neighbours[i].y)) {
						pthread_mutex_unlock(&pl->field_mut);

						pthread_mutex_lock(&pl->queue_mut);
						myqueue_push(pl->queue, neighbours[i]);
						pthread_mutex_unlock(&pl->queue_mut);

						pthread_mutex_lock(&pl->field_mut);
					}
				}
			}
			pthread_mutex_unlock(&pl->field_mut);
			state = WORKING;
		}

		pthread_mutex_lock(&pl->field_mut);
	}
	pthread_mutex_unlock(&pl->field_mut);

	pthread_exit(NULL);
	return NULL;
}

int main(int argc, char* argv[]) {
	if (argc != 4) {
		printf("usage: %s <flower field width> <flower field height> <number of bees>", argv[0]);
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

	pl_t *pl = malloc(sizeof(*pl));
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

	if (pthread_mutex_init(&pl->queue_mut, NULL)) {
		perror("mutex_init");
		return EXIT_FAILURE;
	}

	if (pthread_mutex_init(&pl->field_mut, NULL)) {
		perror("mutex_init");
		return EXIT_FAILURE;
	}

	myqueue_push(pl->queue, (pos_t){.x = 0, .y = 0});
	pthread_t bees[bee_amount];

	for (int i = 0; i < bee_amount; i++) {
		if (pthread_create(&bees[i], NULL, bee_work, pl)) {
			perror("pthread_create");
			return EXIT_FAILURE;	// TODO
		}
	}

	for (int i = 0; i < bee_amount; i++) {
		if (pthread_join(bees[i], NULL)) {
			perror("pthread_create");
			return EXIT_FAILURE;	// TODO
		}
	}

	assert(field_all_is_harvested(pl->field));

	pthread_mutex_destroy(&pl->field_mut);
	pthread_mutex_destroy(&pl->queue_mut);
	field_free(pl->field);
	free(pl);
}
