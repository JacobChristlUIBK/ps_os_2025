#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct field {
	int width;
	int height;
	bool* data;
} t_field;

// initializes a new field.
t_field* init_field(int width, int height) {
	if (width < 1 || height < 1) {
		return NULL;
	}

	t_field* field = malloc(sizeof(*field));
	if (!field) {
		return NULL;
	}

	field->data = malloc(sizeof(*field) * width * height);
	if (!field->data) {
		return NULL;
	}

	field->width = width;
	field->height = height;

	return field;
}

// frees a field.
void free_field(t_field* field) {
	free(field->data);
	free(field);
}

// populates a field according to density.
void populate_field(t_field* field, double density) {
	if (!field) {
		return;
	}

	int threshold = RAND_MAX * density;
	int value = 0;

	for (int i = 0; i < field->width * field->height; i++) {
		value = rand();

		field->data[i] = value < threshold ? true : false;
	}
}

// gets the truth value at a given position on a field.
// returns false if out of bounds or field is NULL.
bool get_value_field(t_field* field, int x, int y) {
	if (!field) {
		return false;
	}

	if (x < 0 || x >= field->width || y < 0 || y >= field->height) {
		return false;
	}

	return field->data[x + y * field->width];
}

// sets the truth value at a given position on a field.
void set_value_field(t_field* field, int x, int y, bool value) {
	if (!field) {
		return;
	}

	if (x < 0 || x >= field->width || y < 0 || y >= field->height) {
		return;
	}

	field->data[x + y * field->width] = value;
}

// counts the neighbours alive at a certain position on a field.
// returns -1 if field is NULL.
int alive_neighbours_field(t_field* field, int x, int y) {
	if (!field) {
		return -1;
	}

	const int directions[8][2] = { { 0, 1 },  { 1, 1 },   { 1, 0 },  { 1, -1 },
		                             { 0, -1 }, { -1, -1 }, { -1, 0 }, { -1, 1 } };
	int count = 0;

	for (int i = 0; i < 8; i++) {
		count += get_value_field(field, x + directions[i][0], y + directions[i][1]) ? 1 : 0;
	}

	return count;
}

// calculates the next step in the game of life.
// does nothing if field or dest are NULL.
void step_field(t_field* field, t_field* dest) {
	if (!field || !dest) {
		return;
	}

	for (int x = 0; x < field->width; x++) {
		for (int y = 0; y < field->height; y++) {
			bool is_alive = get_value_field(field, x, y);
			int alive_neighbours = alive_neighbours_field(field, x, y);

			if (!is_alive && alive_neighbours == 3) {
				set_value_field(dest, x, y, true);
			} else if (!is_alive) {
				set_value_field(dest, x, y, false);
			} else if (is_alive && (alive_neighbours < 2 || 3 < alive_neighbours)) {
				set_value_field(dest, x, y, false);
			} else {
				set_value_field(dest, x, y, true);
			}
		}
	}
}

void write_field(t_field* field, int frame) {
	if (!field) {
		return;
	}

	// open file
	char *path = calloc(14, sizeof(*path));
	sprintf(path, "gol_%05d.pbm", frame);
	FILE* file = fopen(path, "w");
	free(path);

	fprintf(file, "P1\n%d %d", field->width, field->height);

	for (int i = 0; i < field->width * field->height; i++) {
		if (!(i % 76)) {
			putc('\n', file);
		}

		putc(field->data[i] + 48, file);
	}

	// free file
	fclose(file);
}

void printUsage(const char* programName) {
	printf("usage: %s <width> <height> <density> <steps>\n", programName);
}

int main(int argc, char* argv[]) {
	if (argc != 5) {
		printUsage(argv[0]);
		return EXIT_FAILURE;
	}

	const int width = atoi(argv[1]);
	const int height = atoi(argv[2]);
	const double density = atof(argv[3]);
	const int steps = atoi(argv[4]);

	printf("width:   %4d\n", width);
	printf("height:  %4d\n", height);
	printf("density: %4.0f%%\n", density * 100);
	printf("steps:   %4d\n", steps);

	// Seeding the random number generator so we get a different starting field
	// every time.
	srand(time(NULL));

	t_field *field_1 = init_field(width, height);
	t_field *field_2 = init_field(width, height);
	t_field **source = &field_1;

	populate_field(*source, density);

	for (int frame = 0; frame <= steps; frame++) {
		write_field(*source, frame);

		if (*source == field_1) {
			step_field(field_1, field_2);
			source = &field_2;
		} else {
			step_field(field_2, field_1);
			source = &field_1;
		}
	}

	free_field(field_1);
	free_field(field_2);

	return EXIT_SUCCESS;
}
