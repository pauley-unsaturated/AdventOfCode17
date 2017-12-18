/* 
 * Advent of Code: Day 15
 * See README for details
 */
#include <stdio.h>
#include <stdint.h>

#define A_MULT 16807l
#define B_MULT 48271l

#define MODULO 0x7FFFFFFFl

#define NUM_TRIALS 5000000

int main(int argc, const char** argv) {
	int a = 873;
	int b = 583;

	long trials_left = NUM_TRIALS;
	long num_matched = 0;

	while (trials_left-- > 0) {
		do {
			a = (int)((a * A_MULT) % MODULO);
		} while (a % 4 != 0);

		do {
			b = (int)((b * B_MULT) % MODULO);
		} while (b % 8 != 0);
		
		if ( (a & 0xFFFF) == (b & 0xFFFF) ) {
			num_matched++;
		}
	}
	printf("Part 1: %ld\n", num_matched);
	
	return 0;
}
