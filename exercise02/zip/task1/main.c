#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "key.h"

#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define RESET "\033[0m"

int main(void) {
	const char* const username = getenv("USER");
	if (username == NULL) {
		fprintf(stderr,
		        YELLOW "USER environment variable not set. This leads to a wrong output. To correctly "
		               "test your execution set the USER environment variable" RESET "\n");
		return EXIT_FAILURE;
	}

	const char* const key = get_key();

	if (key == NULL) {
		fprintf(stderr, "Key not found\n");
		return EXIT_FAILURE;
	}

	if (strncmp(username, key, strlen(username)) != 0) {
		fprintf(stderr, RED "Invalid key" RESET "\n");
		return EXIT_FAILURE;
	}

	printf(GREEN "Key is valid. Hello, %s!" RESET "\n", username);

	return EXIT_SUCCESS;
}
