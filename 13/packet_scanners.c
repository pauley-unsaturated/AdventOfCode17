/* 
 * Advent of Code Day 13
 * see README for details
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

#define MAX_IDX 4096

int firewall_depths[MAX_IDX] = {0};

int firewall_init_positions[MAX_IDX];
int firewall_init_velocities[MAX_IDX];

int firewall_positions[MAX_IDX];
int firewall_velocities[MAX_IDX];
int max_idx = -1;

int got_caught[MAX_IDX];
int num_got_caught;

int simulate_firewall_with_delay(int delay) {
	int packet_pos = 0;
	int result = 0;
	if (delay % 100 == 0)
		fprintf(stderr, ".");
	// reset the scratch arrays
	memcpy(firewall_positions,  firewall_init_positions, (max_idx + 1) * sizeof(int));
	memcpy(firewall_velocities, firewall_init_velocities, (max_idx + 1) * sizeof(int));
	bzero(got_caught, MAX_IDX * sizeof(int));
	num_got_caught = 0;
	
	while(packet_pos <= max_idx) {
		if(delay <= 0) {
			if(firewall_positions[packet_pos] == 0) {
				result += packet_pos * firewall_depths[packet_pos];
				got_caught[num_got_caught++] = 1;
			}
			packet_pos++;
		}
		else {
			delay--;
		}
		
		for (int i = 0; i <= max_idx; i++) {
			if ((firewall_positions[i] == firewall_depths[i] - 1
					 && firewall_velocities[i] > 0)
					|| (firewall_positions[i] == 0
							&& firewall_velocities[i] < 0)) {
				firewall_velocities[i] *= -1;
			}
			firewall_positions[i] += firewall_velocities[i];
		}
	}
	return result;
}

int main (int argc, const char** argv) {
	FILE* input;

	if (argc > 1) {
		input = fopen(argv[1], "r");
	}
	else {
		input = stdin;
	}

	char* line;
	size_t line_len = 0;

	for (int i = 0; i < MAX_IDX; i++) {
		firewall_init_positions[i] = -1;
	}
	
	while(NULL != (line = fgetln(input, &line_len))) {
		int firewall_idx;
		int firewall_level;
		line[line_len - 1] = '\0';
		sscanf(line, "%d: %d", &firewall_idx, &firewall_level);
		assert(firewall_idx < MAX_IDX);
		assert(firewall_level < MAX_IDX);

		if (firewall_idx > max_idx) max_idx = firewall_idx;
		firewall_depths[firewall_idx] = firewall_level;
		firewall_init_positions[firewall_idx] = 0;
		firewall_init_velocities[firewall_idx] = 1;
	}

	int result = simulate_firewall_with_delay(0);
	printf("Part 1: %d\n", result);


	// This is won't work if the input is big enough,
	//  I appear to be doing an exponential amount of work.
	/*
	int delay = 0;
	for(int delay = 0;;delay++) {
		simulate_firewall_with_delay(delay);
		if (num_got_caught == 0) {
				printf("Part 2: %d\n", delay);
				break;
		}
	}
	*/
	int delay = 1;
	for(;;) {
		bool success = true;
		for(int i = 0; i <= max_idx; i++) {
			if (firewall_depths[i] > 0) {
				if( (delay + i) % ((firewall_depths[i] - 1) * 2) == 0) {
					success = false;
				}
			}
		}
		if (success) {
			printf("Part 2: %d\n", delay);
			break;
		}
		delay++;
	}
	
	fclose(input);
	return 0;
}
