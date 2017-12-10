/*
--- Day 6: Memory Reallocation ---

A debugger program here is having an issue: it is trying to repair a memory 
reallocation routine, but it keeps getting stuck in an infinite loop.

In this area, there are sixteen memory banks; each memory bank can hold any 
number of blocks. The goal of the reallocation routine is to balance the blocks 
between the memory banks.

The reallocation routine operates in cycles. In each cycle, it finds the memory 
bank with the most blocks (ties won by the lowest-numbered memory bank) and 
redistributes those blocks among the banks. To do this, it removes all of the 
blocks from the selected bank, then moves to the next (by index) memory bank and
inserts one of the blocks. It continues doing this until it runs out of blocks;
if it reaches the last memory bank, it wraps around to the first one.

The debugger would like to know how many redistributions can be done before a b
locks-in-banks configuration is produced that has been seen before.

For example, imagine a scenario with only four memory banks:

The banks start with 0, 2, 7, and 0 blocks. The third bank has the most blocks, 
so it is chosen for redistribution.
Starting with the next bank (the fourth bank) and then continuing to the first 
bank, the second bank, and so on, the 7 blocks are spread out over the memory 
banks. The fourth, first, and second banks get two blocks each, and the third 
bank gets one back. The final result looks like this: 2 4 1 2.
Next, the second bank is chosen because it contains the most blocks (four). 
Because there are four memory banks, each gets one block. 
The result is: 3 1 2 3.
Now, there is a tie between the first and fourth memory banks, both of which 
have three blocks. The first bank wins the tie, and its three blocks are 
distributed evenly over the other three banks, leaving it with none: 0 2 3 4.
The fourth bank is chosen, and its four blocks are distributed such that each of
the four banks receives one: 1 3 4 1.
The third bank is chosen, and the same thing happens: 2 4 1 2.
At this point, we've reached a state we've seen before: 2 4 1 2 was already 
seen. The infinite loop is detected after the fifth block redistribution cycle, 
and so the answer in this example is 5.

Given the initial block counts in your puzzle input, how many redistribution 
cycles must be completed before a configuration is produced that has been seen 
before?
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

int banks[] = {
	// Problem input
	4, 1,	15,	12,	0, 9,	9, 5,	5, 8,	7, 3,	14,	5, 12, 3
	// Example input
	//0, 2, 7, 0
};

int max_idx(size_t len, int *list) {
	assert(len > 0);
	assert(list != NULL);
	int result = 0;
	int max = list[0];
	for (size_t i = 1; i < len; i++) {
		if (list[i] > max) {
			result = i;
			max = banks[i];
		}
	}
	return result;
}

unsigned long hash(size_t len, int *banks) {
	/* start here, LOL*/
	unsigned long hash = 5381;
	int c;
	
	while (len-- > 0) {
		c = *banks++;
		/* hash * 33 + c */
		hash = ((hash << 5) + hash) + c; 
	}
	
	return hash;
}

void dump(size_t len, int *banks) {
	for(int i = 0; i < (int)len; i++) {
		printf("%d ", banks[i]);
	}
	printf("\n");
}

bool contains_hash(unsigned long *haystack, unsigned long needle) {
	unsigned long* cur = haystack;
	while(*cur) {
		if (*cur++ == needle) return true;
	}
	return false;
}

unsigned long hashes[1024*1024] = {0}; // 1MB should be enough!

int main(int argc, const char** argv) {
	const size_t banks_len = (sizeof banks) / sizeof(banks[0]);
		

	unsigned long cur_hash;
	char buf[sizeof(unsigned long) + 1];

	cur_hash = hash(banks_len, banks);

	unsigned long *next_hash = hashes;
	int num_iterations = 0;
	
	for(;;) {
		//printf("0x%lx ", cur_hash);
		//dump(banks_len, banks);
		*next_hash++ = cur_hash;
		int idx = max_idx(banks_len, banks);
		int val = banks[idx];
		banks[idx] = 0;

		while(val-- > 0) {
			idx++;
			if (idx >= (int)banks_len) idx = 0;
			banks[idx]++;
		}
		num_iterations++;
		
		cur_hash = hash(banks_len, banks);
		if(contains_hash(hashes, cur_hash)) {
			break;
		}
	}
	printf("0x%lx ", cur_hash);
	dump(banks_len, banks);
	printf("%d\n", num_iterations);
	unsigned long *test = &hashes[num_iterations - 1];
	int i = 1;
	while (*test-- != cur_hash) i++;
	printf("loop len = %d\n", i);

	return 0;
}

