#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

enum exit_codes {
	EXIT_NO_ARGS,
	EXIT_ARG_OVERFLOW,
	EXIT_INVALID_OFFSET,
	EXIT_NO_OFFSET,
};

int main(int argc, char* args[]) {
	const char *var = "OFFSET";
	char* endptr;
	char* offset_str = getenv(var);

	if (offset_str == NULL) {
		fprintf(stderr, "error: environment variable OFFSET not set");
	}

	long offset = strtol(offset_str, &endptr, 10);

	printf("%s\n", offset_str);

	if (!(*offset_str != '\0' && *endptr == '\0')) {
		fprintf(stderr, "error: invalid offset, set environment variable OFFSET to a number");
		return EXIT_INVALID_OFFSET;
	}

	long argcount = argc + offset - 1;

	if (argcount == 0) {
		fprintf(stderr, "usage: %s <arg1> [arg2 ... arg10]\n", args[0]);
		return EXIT_NO_ARGS;
	} else if (argcount > 10) {
		fprintf(stderr, "error: more than 10 arguments provided");
		return EXIT_ARG_OVERFLOW;
	}

	printf("result: %ld\n", argcount);
	return EXIT_SUCCESS;
}
