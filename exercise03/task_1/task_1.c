#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DBG_SIG(sig) printf("Recieved %s, signal number: %d\n", #sig, sig)

volatile sig_atomic_t int_res = 0;
volatile sig_atomic_t stop_res = 0;
volatile sig_atomic_t cont_res = 0;
volatile sig_atomic_t kill_res = 0;
volatile sig_atomic_t usr1_res = 0;
volatile sig_atomic_t usr2_res = 0;

void handle_signal(int sig_num) {
	// set global flags to print recieved signals in main thread, since printf isn't async-async-safe
	switch (sig_num) {
		case SIGINT: int_res++; break;
		case SIGSTOP: stop_res++; break;
		case SIGCONT: cont_res++; break;
		case SIGKILL: kill_res++; break;
		case SIGUSR1: usr1_res++; break;
		case SIGUSR2: usr2_res++; break;
	}
}

// the curly brackets around the 0 are required to remove some warning.
struct sigaction default_action = { .sa_handler = &handle_signal, .sa_mask = {{0}}, .sa_flags = 0 };
struct sigaction no_action = { .sa_handler = SIG_IGN, .sa_mask = {{0}}, .sa_flags = 0 };

void print_recieved_signals() {
	while (int_res) {
		if (sigaction(SIGUSR1, &default_action, NULL) == -1) {
			perror(NULL);
		}

		if (sigaction(SIGUSR2, &default_action, NULL) == -1) {
			perror(NULL);
		}

		DBG_SIG(SIGINT);
		int_res--;
	}
	while (stop_res) {
		DBG_SIG(SIGSTOP);
		stop_res--;
	}
	while (cont_res) {
		DBG_SIG(SIGCONT);
		cont_res--;
	}
	while (kill_res) {
		DBG_SIG(SIGKILL);
		kill_res--;
	}
	while (usr1_res) {
		if (sigaction(SIGUSR2, &no_action, NULL) == -1) {
			perror(NULL);
		}

		DBG_SIG(SIGUSR1);
		usr1_res--;
	}
	while (usr2_res) {
		if (sigaction(SIGUSR1, &no_action, NULL) == -1) {
			perror(NULL);
		}

		DBG_SIG(SIGUSR2);
		usr2_res--;
	}
}

int main(void) {
	if (sigaction(SIGINT, &default_action, NULL) == -1) {
		perror("error creating sigaction SIGINT");
	}

	if (sigaction(SIGCONT, &default_action, NULL) == -1) {
		perror("error creating sigaction SIGCONT");
	}

	if (sigaction(SIGUSR1, &default_action, NULL) == -1) {
		perror("error creating sigaction SIGUSR1");
	}

	if (sigaction(SIGUSR2, &default_action, NULL) == -1) {
		perror("error creating sigaction SIGUSR2");
	}

	// sigaction(2) defines these as invalid:
	//
	// if (sigaction(SIGKILL, &act, NULL) == -1) {
	// 	perror("error creating sigaction SIGKILL");
	// }
	//
	// if (sigaction(SIGSTOP, &act, NULL) == -1) {
	// 	perror("error creating sigaction SIGSTOP");
	// }

	const time_t work_seconds = 1;

	struct timespec work_duration = {
		.tv_sec = work_seconds,
	};

	struct timespec remaining = { 0 };

	while (true) {
		// simulate real workload
		if (nanosleep(&work_duration, &remaining) == -1 && errno == EINTR) {
			work_duration = remaining;
			continue;
		}

		// restore work_duration
		work_duration.tv_sec = work_seconds;

		print_recieved_signals();
	}

	return EXIT_SUCCESS;
}
