#include "my_pthread_barrier.h"
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Holds player data.
typedef struct {
	int result;
	bool playing;
	pthread_t thread;
} player_t;

// Holds the payload provided to each thread.
typedef struct {
	int id;
	int player_count;
	int* active_players;
	player_t** players;
	my_pthread_barrier_t* barrier;
} payload_t;

// Returns a number between 2 and 12.
int roll() {
	int sum = 0;

	sum += random() % 6 + random() % 6 + 2;

	return sum;
}

// Returns a the minimum result of all players in an array.
int min_result(player_t** players, int player_count) {
	int res = INT_MAX;

	for (int i = 0; i < player_count; i++) {
		if (players[i]->playing && players[i]->result < res) {
			res = players[i]->result;
		}
	}

	return res;
}

// Returns a the maximum result of all players in an array.
int max_result(player_t** players, int player_count) {
	int res = INT_MIN;

	for (int i = 0; i < player_count; i++) {
		if (players[i]->playing && players[i]->result > res) {
			res = players[i]->result;
		}
	}

	return res;
}

int get_winner_id(player_t** players, int player_count) {
	for (int i = 0; i < player_count; i++) {
		if (players[i]->playing) {
			return i;
		}
	}

	return -1;
}

void eliminate_players(payload_t* pl) {
	int min = min_result(pl->players, pl->player_count);
	int max = max_result(pl->players, pl->player_count);

	if (min == max) {
		printf("Repeating round\n");
	} else {
		for (int i = 0; i < pl->player_count; i++) {
			if (pl->players[i]->playing && pl->players[i]->result == max) {
				printf("Eliminating player %d\n", i);
				pl->players[i]->playing = false;
				*(pl->active_players) = *(pl->active_players) - 1;
			}
		}
	}

	printf("----------------------------------\n");
}

// Function to be passed to threads.
// Gets a `*payload_t` passed.
void* run_player(void* pl_) {
	if (!pl_) {
		pthread_exit(NULL);
	}
	payload_t* pl = (payload_t*)pl_;

	int winner = -1;
	int res;

	while (1) {
		// prevent losing players from rolling before they are eliminated and writing to `pl->active_players` before reading it
		my_pthread_barrier_wait(pl->barrier);

		if (*(pl->active_players) == 1) {
			break;
		}

		if (pl->players[pl->id]->playing) {
			pl->players[pl->id]->result = roll();
			printf("Player %d rolled a %d\n", pl->id, pl->players[pl->id]->result);
		}

		res = my_pthread_barrier_wait(pl->barrier);

		if (res == MY_PTHREAD_BARRIER_SERIAL_THREAD) {
			eliminate_players(pl);
		}
	}

	if (res == MY_PTHREAD_BARRIER_SERIAL_THREAD) {
		winner = get_winner_id(pl->players, pl->player_count);

		if (winner == -1) {
			printf("error: something went wrong with deciding the winner.\n");
			pthread_exit(NULL);
		}

		printf("Player %d has won the game\n", winner);
	}

	pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		printf("usage: %s <player_count>", argv[0]);
		return EXIT_SUCCESS;
	}

	int player_count = atoi(argv[1]);
	if (player_count < 2) {
		fprintf(stderr, "error: player count has to be at least 2.");
		return EXIT_FAILURE;
	}

	payload_t* payloads[player_count];
	player_t* players[player_count];
	int active_players = player_count;

	// init barrier
	my_pthread_barrier_t barrier;
	my_pthread_barrier_init(&barrier, NULL, player_count);

	srandom(time(NULL));

	for (int i = 0; i < player_count; i++) {
		players[i] = malloc(sizeof(player_t));
		if (!players[i]) {
			for (int j = 0; j < i; j++) {
				free(players[j]);
				free(payloads[j]);
			}

			perror("malloc");
			return EXIT_FAILURE;
		}

		players[i]->result = 0;
		players[i]->playing = true;

		payloads[i] = malloc(sizeof(payload_t));
		if (!payloads[i]) {
			for (int j = 0; j <= i; j++) {
				free(players[j]);
				free(payloads[j]);
			}

			perror("malloc");
			return EXIT_FAILURE;
		}

		payloads[i]->id = i;
		payloads[i]->player_count = player_count;
		payloads[i]->players = players;
		payloads[i]->barrier = &barrier;
		payloads[i]->active_players = &active_players;

		int res = pthread_create(&payloads[i]->players[i]->thread, NULL, run_player, payloads[i]);
		if (res < 0) {
			for (int j = 0; j <= i; j++) {
				free(players[j]);
				free(payloads[j]);
			}

			perror("pthread_create");
			return EXIT_FAILURE;
		}
	}

	for (int i = 0; i < player_count; i++) {
		int res = pthread_join(payloads[i]->players[i]->thread, NULL);
		if (res < 0) {
			for (int j = 0; j < player_count; j++) {
				free(players[j]);
				free(payloads[j]);
			}

			perror("pthread_join");
			return EXIT_FAILURE;
		}

		free(players[i]);
		free(payloads[i]);
	}

	my_pthread_barrier_destroy(&barrier);
	return EXIT_SUCCESS;
}
