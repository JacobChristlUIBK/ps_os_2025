#include <stdbool.h>

#ifndef SLOT_MACHINE_H
#define SLOT_MACHINE_H

#define NUM_REELS 3

// Slot machine symbol type
typedef char* SlotSymbol;

typedef struct Reel {
	int num_reel_entries;
	SlotSymbol reel[];
} Reel;

typedef struct SlotMachine {
	int num_reels;
	Reel* reels[];
} SlotMachine;

SlotMachine* create_slot_machine();

SlotSymbol get_random_spin_result(Reel *reel, int random_number);

bool are_spin_results_equal(SlotSymbol a, SlotSymbol b);

double get_all_equal_probability();

void destroy_slot_machine(SlotMachine *slot_machine);

#endif
