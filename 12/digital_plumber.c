/* 
  Advent of code 2017
  Day 12
  See README for details
  Author: Mark Pauley
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <stdbool.h>

#define MAX_NODES 4096
#define INIT -1

int nodes[MAX_NODES];;
int num_nodes = 0;

int get_root(int node) {
	while(nodes[node] != node) {
		node = nodes[node];
	}
	return node;
}

bool same_root(int a, int b) {
	return (get_root(a) == get_root(b));
}

int main(int argc, const char** argv) {
	FILE* input;

	if (argc < 2) {
		input = stdin;
	}
	else {
		input = fopen(argv[1], "r");
	}

	// Basically this problem is UNION-FIND
	// We'll use the lazy version as given in
	// Algorithms v4 by Robert Sedgewick
	for (int i = 0; i < MAX_NODES; i++) {
		nodes[i] = i;
	}
	// Get the number of nodes
	char* line;
	size_t line_len = 0;
	while(NULL != (line = fgetln(input, &line_len))) {
		char* cur = line;
		char* begin = line;
		line[line_len - 1] = '\0';

		// Parse the node number
		while(isdigit(*cur))cur++;
		assert(isspace(*cur));
		*cur = '\0';
		int target = atoi(line);
		
		// Skip til the beginning of links
		line = cur + 1;

		// Parse the links
		while ((size_t)(line - begin) < line_len) {
			int link = INIT;
			
			while (!isdigit(*line))line++;
			cur = line;
			while (isdigit(*cur))cur++;
			*cur = '\0';

			link = atoi(line);
			
			if(!same_root(link, target)) {
				int target_root = get_root(target);
				int link_root = get_root(link);
				if (link_root < target_root) {
					nodes[target_root] = link_root;
				}
				else {
					nodes[link_root] = target_root;
				}
			}
			line = cur + 1;
		}
		num_nodes++;
	}

	printf("Read %d nodes\n", num_nodes);

	int root = get_root(0);
	int num_in_group = 0;
	for (int i = 0; i < num_nodes; i++) {
		if (get_root(i) == root) {
			num_in_group++;
		}
	}
	printf("Part 1: %d\n", num_in_group);

	int groups[1024];
	int num_groups = 0;
	for (int i = 0; i < num_nodes; i++) {
		int root = get_root(i);
		bool found = false;
		for (int j = 0; j < num_groups; j++) {
			if (groups[j] == root) {
				found = true;
				break;
			}
		}
		if (!found) {
			groups[num_groups++] = root;
		}
	}

	printf("Part 2: %d\n", num_groups);
	
	
	return 0;
}
