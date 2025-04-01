#include "sched.h"
#include "sys/wait.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>

int main(int argc, char* argv[]) {
	if (argc != 3) {
		fprintf(stderr, "usage: %s <file1> <file2>", argv[0]);
		return EXIT_FAILURE;
	}

	int pipefd[2];
	char buf[10];
	pid_t pid;

	if (pipe(pipefd) == -1) {
		err(EXIT_FAILURE, "pipe");
	}

	pid = fork();

	if (pid == -1) {
		err(EXIT_FAILURE, "fork");
	}

	fflush(stdout);

	if (pid == 0) {
		if (close(pipefd[1]) == -1) {
			err(EXIT_FAILURE, "close");
		}

		printf("a");
		fflush(stdout);

		FILE *f = fdopen(pipefd[0], "r");

		printf("%.*s\n", fget, buf);
		fflush(stdout);

		fclose(f);

		// execl("cut", "-c", "22-", buf, (char *) NULL);
		exit(EXIT_SUCCESS);
	} else {
		if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
			err(EXIT_FAILURE, "dup2");
		}

		if (close(pipefd[0]) == -1 || close(pipefd[1]) == -1 ) {
			err(EXIT_FAILURE, "close");
		}

		execl("cat", argv[1], argv[2], (char *) NULL);

		wait(NULL);
		fprintf(stderr, "c");
		fflush(stderr);
	}

	return EXIT_SUCCESS;
}
