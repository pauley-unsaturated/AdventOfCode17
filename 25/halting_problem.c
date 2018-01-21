/*
 * Advent of Code: Day 25 
 * See README for details
 */

/* 
 * Note:
 * Instead of doing the labor-intensive work of writing a full
 * Turing Machine parser in C, we are going to just hard-code
 * the description of the machine given in input.txt
 */

#include <stdbool.h>
#include <stdio.h>

typedef bool Cell;

#define NUM_CELLS (1024 * 1024)
Cell cells[NUM_CELLS];
Cell* curCell = cells + (NUM_CELLS / 2);

typedef enum {
	A, B, C, D, E, F
} State;

typedef State (*StateFunc)(Cell** cur);

#define DO_STATE(x) State DoState_ ## x (Cell** c)
#define DECL_STATE(x) DO_STATE(x);

DECL_STATE(A)
DECL_STATE(B)
DECL_STATE(C)
DECL_STATE(D)
DECL_STATE(E)
DECL_STATE(F)

#define MACHINE_STATE(x) [x] = DoState_ ## x
StateFunc FSM[] = {
	/* Flying Spaghetti Monster */
	MACHINE_STATE(A),
	MACHINE_STATE(B),
	MACHINE_STATE(C),
	MACHINE_STATE(D),
	MACHINE_STATE(E),
	MACHINE_STATE(F)
};

#define RUN_STATE(x, cur) FSM[(x)](&cur)

long int num_set = 0;

#define CURRENT **c
#define WRITE(x) do { \
		if(!x && CURRENT)\
			num_set--;\
		else if(x && !CURRENT)\
			num_set++;\
		CURRENT = (x); \
}while(0)

#define LEFT -1
#define RIGHT 1
#define MOVE(x) (*c) += (x)

DO_STATE(A) {
	if (CURRENT == 0) {
		WRITE(true);
		MOVE(RIGHT);
		return B;
	}
	else {
		WRITE(false);
		MOVE(LEFT);
		return B;
	}
}

DO_STATE(B) {
	if (CURRENT == 0) {
		WRITE(true);
		MOVE(LEFT);
		return C;
	}
	else {
		WRITE(false);
		MOVE(RIGHT);
		return E;
	}
}

DO_STATE(C) {
	if (CURRENT == 0) {
		WRITE(true);
		MOVE(RIGHT);
		return E;
	}
	else {
		WRITE(false);
		MOVE(LEFT);
		return D;
	}
}

DO_STATE(D) {
	if (CURRENT == 0) {
		WRITE(true);
		MOVE(LEFT);
		return A;
	}
	else {
		WRITE(1);
		MOVE(LEFT);
		return A;
	}
}

DO_STATE(E) {
	if (CURRENT == 0) {
		WRITE(false);
		MOVE(RIGHT);
		return A;
	}
	else {
		WRITE(false);
		MOVE(RIGHT);
		return F;
	}
}

DO_STATE(F) {
	if (CURRENT == 0) {
		WRITE(true);
		MOVE(RIGHT);
		return E;
	}
	else {
		WRITE(true);
		MOVE(RIGHT);
		return A;
	}
}

#define STARTING_STATE (State)A
#define NUM_STEPS 12861455l

int main(int argc, const char** argv) {
	State cur_state = STARTING_STATE;
	
	for (long i = 0; i < NUM_STEPS; i++) {
		cur_state = RUN_STATE(cur_state, curCell);
	}
	printf("Part 1: %ld\n", num_set);
	return 0;
}
