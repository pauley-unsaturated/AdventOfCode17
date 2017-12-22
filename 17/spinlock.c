/*
 * Advent of code: Day 17
 * See README for details
 */

#include <stdio.h>


typedef struct node {
	int val;
	struct node* next;
} Node;

#define NUM_NODES 2018
#define SKIP 328
Node free_list[NUM_NODES];

int main(int argc, const char** argv) {
	int i = 0;
	Node* cur = free_list;
	cur->next = cur;
	cur->val = i++;

	while(i < NUM_NODES) {
		for(int j = 0; j < SKIP; j++) {
			cur = cur->next;
		}
		Node* prev = cur;
		Node* next = cur->next;
		cur = free_list + i;
		prev->next = cur;
		cur->next = next;
		cur->val = i;
		i++;
	}

	printf("Part 1: %d -> %d\n", cur->val, cur->next->val);
	return 0;
}


