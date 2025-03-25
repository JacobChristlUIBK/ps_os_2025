#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

// Some reformatting for better readability.

void string_shift_right(int arg_index, char* input, const char* original) {
	size_t input_length = strlen(input);
	// strlen doesn't account for the null character
	// char shifted_input[input_length];
	char shifted_input[input_length + 1];
	// add null terminator
	shifted_input[input_length] = '\0';

	for (size_t char_index = 0; char_index < input_length; ++char_index) {
		// use modulo to get rotational shift instead of division
		// size_t new_position = (char_index + 2) / input_length;
		size_t new_position = (char_index + 2) % input_length;
		shifted_input[new_position] = input[char_index];
	}

	for (size_t char_index = 0; char_index < input_length; ++char_index) {
		input[char_index] = toupper(input[char_index]);
	}

	printf("original argv[%d]: %s\nuppercased: %s\nshifted: %s\n", arg_index, original, input,
	       shifted_input);
}

int main(int argc, const char** argv) {
	char* shared_strings[argc];

	for (int arg_index = 0; arg_index < argc; ++arg_index) {
		// again strlen doesnt account for null terminator
		// size_t arg_length = strlen(argv[arg_index]);
		size_t arg_length = strlen(argv[arg_index]) + 1;

		shared_strings[arg_index] = malloc(arg_length * sizeof(char));
		if (shared_strings[arg_index] == NULL) {
			fprintf(stderr, "Could not allocate memory.\n");
			exit(EXIT_FAILURE);
		}

		strcpy(shared_strings[arg_index], argv[arg_index]);
	}

	// boundary of <= is one iteration too much
	// for (int arg_index = 0; arg_index <= argc; arg_index++) {
	for (int arg_index = 0; arg_index < argc; arg_index++) {
		pid_t pid = fork();

		if (pid == -1) {
			perror("Could not create fork");
			exit(EXIT_FAILURE);
		}

		if (pid == 0) {
			string_shift_right(arg_index, shared_strings[arg_index], argv[arg_index]);
			exit(EXIT_SUCCESS);
		}
	}

	// again boundary of <= is too high
	// for (int arg_index = 0; arg_index <= argc; arg_index++) {
	for (int arg_index = 0; arg_index < argc; arg_index++) {
		wait(NULL);
	}

	for (int arg_index = 0; arg_index < argc; arg_index++) {
		free(shared_strings[arg_index]);
	}

	printf("Done.");
	return EXIT_SUCCESS;
}
