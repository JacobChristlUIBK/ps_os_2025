#include "fcntl.h"
#include "mqueue.h"
#include "unistd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
	if (argc != 3) {
		fprintf(stderr, "usage: %s <queue> <priority>\n", argv[0]);
		return EXIT_FAILURE;
	}

	if (strchr(argv[1] + 1, '/')) {
		perror("message queue name must not contain slashes after the first character\n");
		return EXIT_FAILURE;
	}

	char stdin_buf[8192];
	int read_bytes = read(STDIN_FILENO, stdin_buf, 8192);
	if (read_bytes == -1) {
		perror("failed to read stdin");
		return EXIT_FAILURE;
	}

	stdin_buf[read_bytes] = '\0';

	mqd_t mqd = mq_open(argv[1], O_WRONLY);
	if (mqd == -1) {
		perror("could not open message queue");
		return EXIT_FAILURE;
	}

	if (mq_send(mqd, stdin_buf, strlen(stdin_buf), atoi(argv[2])) == -1) {
		perror("could not send message to queue");
	}

	return EXIT_SUCCESS;
}
