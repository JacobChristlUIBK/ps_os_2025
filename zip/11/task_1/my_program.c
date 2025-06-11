#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

void print_array(int* array, size_t size) {
	for (size_t i = 0; i < size; i++) {
		printf("%d", array[i]);

		if (i == size - 1) {
			printf("\n");
		} else {
			printf(", ");
		}
	}
}

void bubble_sort(int numbers[], int size) {
	for (int i = 0; i < size - 1; ++i) {
		int s = 0;

		for (int j = 0; j < size - i - 1; ++j) {
			if (numbers[j] > numbers[j + 1]) {
				int temp = numbers[j];
				numbers[j] = numbers[j + 1];
				numbers[j + 1] = temp;
				s = 1;
			}
		}

		if (s == 0) {
			break;
		}
	}
}

int main(int argc, char* argv[]) {
	int* numbers = malloc(sizeof(*numbers) * (argc - 1));

	for (int i = 1; i < argc; i++) {
		numbers[i - 1] = atoi(argv[i]);
	}

	printf("Input numbers:\n");
	print_array(numbers, argc - 1);

	bubble_sort(numbers, argc - 1);

	printf("Output numbers:\n");
	print_array(numbers, argc - 1);

	free(numbers);
	return EXIT_SUCCESS;
}
