/*
 * Advent of Code: Day 24
 * See README for details
 */


#include <stdio.h>

typedef struct _component {
	int first;
	int second;
	struct _component* next;
} component;

#define NUM_COMPONENTS 128
component all_components[NUM_COMPONENTS];
size_t components_count = 0;

component* free_components;

int max_depth = 0;
int strongest_max_depth = 0;

int strongest_bond(int depth, int strength, int match, component* components) {
	int max_result = strength;

	if (depth > max_depth) {
		max_depth = depth;
		strongest_max_depth = strength;
	}
	else if (depth == max_depth && strength > strongest_max_depth) {
		strongest_max_depth = strength;
	}

	component* cur = components;
	component* prev = NULL;
	while(cur != NULL) {
		int matched = -1;
		int to_match = -1;
		if (cur->first == match) {
			matched = cur->first;
			to_match = cur->second;
		}
		else if (cur->second == match) {
			matched = cur->second;
			to_match = cur->first;
		}
		if (matched != -1) {

			//for(int i = 0; i < depth; i++) putc('\t', stdout);
			//printf("%d/%d\n", cur->first, cur->second);
			
			int result = strength + matched + to_match;
			if (prev) {
				// remove this one
				prev->next = cur->next;
				result = strongest_bond(depth + 1, result, to_match, components);
				// put it back in
				cur->next = prev->next;
				prev->next = cur;
			}
			else {
				// this is the first, just skip it to get the recursive case
				result = strongest_bond(depth + 1, result, to_match, cur->next);
			}
			if (result > max_result) {
				max_result = result;
			}

			
		}
		prev = cur;
		cur = cur->next;
	}
	
	return max_result;
}

int main(int argc, const char** argv) {
	FILE* in;
	if (argc > 1) {
		in = fopen(argv[1], "r");
	}
	else {
		in = stdin;
	}

	// Parse the input
	component* cur = all_components;
	component* end = all_components + NUM_COMPONENTS;	
	component* tail = NULL;

	if (2 != fscanf(in, "%d/%d\n", &(cur->first), &(cur->second))) {
		fprintf(stderr, "Invalid input file!\n");
		return 1;
	}
	// Initialize the first one
	free_components = cur;
	cur->next = NULL;
	tail = free_components;
	cur++;

	// Chain them together
	while(cur != end && 2 == fscanf(in, "%d/%d\n", &(cur->first), &(cur->second))) {
		cur->next = NULL;
		tail->next = cur;
		tail = cur;		
		cur++;
	}

	components_count = cur - all_components;
	printf("Read %zu components\n", components_count);
	int result = strongest_bond(0, 0, 0, free_components);
	printf("Part 1: %d\n", result);

	printf("Part 2: %d\n", strongest_max_depth);
	
	return 0;
}
