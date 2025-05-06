#include "slot_machine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define NUM_SYMBOLS 10

// Define the slot machine with three reels (i.e. "wheels") and the same ten symbols for each reel.
SlotMachine* create_slot_machine() {
	SlotSymbol symbols[] = {
		"⭐",
		"💀",
		"7️⃣ ",
		"👑",
		"🍒",
		"🍋",
		"🔔",
		"💎",
		"🍉",
		"🍊",
	};

	SlotMachine *slot_machine = malloc(sizeof(SlotMachine) + NUM_REELS * sizeof(Reel));
	slot_machine->num_reels = NUM_REELS;

	for (int i = 0; i < NUM_REELS; i++) {
		Reel *reel = malloc(sizeof(Reel) + NUM_SYMBOLS * sizeof(SlotSymbol));
		for (int j = 0; j < NUM_SYMBOLS; j++) {
			reel->reel[j] = malloc(strlen(symbols[j]) + 1);  // +1 for '\0'
			strcpy(reel->reel[j], symbols[j]);
		}

		reel->num_reel_entries = NUM_SYMBOLS;
		slot_machine->reels[i] = reel;
	}

	return slot_machine;
}

SlotSymbol get_random_spin_result(Reel *reel, int random_number) {
	int spin_result_index = random_number % reel->num_reel_entries;
	return reel->reel[spin_result_index];
}

bool are_spin_results_equal(SlotSymbol a, SlotSymbol b) {
	return strcmp(a, b) == 0;
}

double get_all_equal_probability() {
	// naive calculation based on create_slot_machine above.
	double probability = 1.0;
	double probability_of_one_symbol = 1.0 / NUM_SYMBOLS;
	for (int i = 1; i < NUM_REELS; i++) {
		probability *= probability_of_one_symbol;
	}

	return probability;
}

void destroy_slot_machine(SlotMachine *slot_machine) {
	int num_reels = slot_machine->num_reels;
	for (int i = 0; i < num_reels; i++) {
		Reel *reel = slot_machine->reels[i];

		for (int j = 0; j < reel->num_reel_entries; j++) {
			free(reel->reel[j]);
		}
		free(reel);
	}
	free(slot_machine);
}