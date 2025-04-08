#define _DEFAULT_SOURCE

#include "signal.h"
#include <fcntl.h>
#include <limits.h>
#include <mqueue.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>
#include <unistd.h>

#define MSG_SIZE 8192

int shutdown = 0;

void handle_signal(int sig_num) {
	if (sig_num == SIGINT) {
		shutdown = 1;
	}
}

struct sigaction sig_action = { .sa_handler = &handle_signal, .sa_mask = { { 0 } }, .sa_flags = 0 };

unsigned token_count(char* str) {
	int offset = 0, count = 1;

	while (str[offset] != '\0') {
		if (str[offset] == ' ') {
			count++;
		}

		offset++;
	}

	return count;
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		printf("usage: %s <queue>\n", argv[0]);
		return EXIT_FAILURE;
	}

	if (strchr(argv[1] + 1, '/')) {
		perror("message queue name must not contain slashes after the first character");
		return EXIT_FAILURE;
	}

	if (sigaction(SIGINT, &sig_action, NULL) == -1) {
		perror("failed to create sigaction");
		return EXIT_FAILURE;
	}

	mqd_t mqd = mq_open(argv[1], O_RDONLY | O_CREAT);
	if (mqd == -1) {
		perror("could not open message queue");
		return EXIT_FAILURE;
	}

	char msg_buf[MSG_SIZE], *token, *save_ptr;
	unsigned int prio;

	int max, min, sum, count, token_amount;

	while (1) {
		if (mq_receive(mqd, msg_buf, MSG_SIZE, &prio) == -1) {
			perror("failed to recieve message");
		}

		printf("Scheduling task with priority %u\n", prio);

		max = INT_MIN;
		min = INT_MAX;
		sum = 0;
		count = 0;
		token_amount = token_count(msg_buf);
		token = strtok_r(msg_buf, " ", &save_ptr);

		printf("test");
		fflush(stdout);

		while (token) {
			int val = atoi(token);

			max = val > max ? val : max;
			min = val < min ? val : min;
			sum += val;
			count++;

			token = strtok_r(NULL, " ", &save_ptr);

			if (thrd_sleep(&(struct timespec){ .tv_nsec = 500000000 }, NULL) < 0) {
				perror("sleep");
				return EXIT_FAILURE;
			}

			printf("\rStatistics progress: %d%%", (100 * (count) / token_amount));
			fflush(stdout);
		}
		printf("\nMax: %d, Min: %d, Mean: %f\n", max, min, (float)sum / (float)token_amount);

		if (shutdown == 1) {
			break;
		}
	}

	if (mq_close(mqd) == -1) {
		perror("failed to unlink");
		return EXIT_FAILURE;
	}

	if (mq_unlink(argv[1]) == -1) {
		perror("failed to unlink");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
