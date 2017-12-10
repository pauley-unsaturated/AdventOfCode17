/*
 * Day 9 solution, see Part_1 and Part_2 text files
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

typedef enum _state {
	normal,
	garbage
} State;

int main(int argc, const char** argv) {
	FILE* input;
	if (argc < 2) {
		input = stdin;
	}
	else {
		input = fopen(argv[1], "r");
	}
	
	int cur_level = 0;
	int cur_sum = 0;
	int num_garbage = 0;
	bool cancelled = false; 
	State state = normal;

	int c;
	while(EOF != (c = getc(input))) {
		if (cancelled) {
			cancelled = false;
			continue;
		}
		
		switch (state) {
		case normal:
			switch (c) {
			case '{':
				cur_level++;
				break;
			case '}':
				cur_sum += cur_level--;
				assert(cur_level >= 0);
				break;
			case '<':
				state = garbage;
				break;
			case '!':
				cancelled = true;
				break;
			}
			break;
		case garbage:
			switch (c) {
			case '>':
				state = normal;
				break;
			case '!':
				cancelled = true;
				break;
			default:
				num_garbage++;
				break;
			}
			break;
		}
	}

	printf("%d\n", cur_sum);
	printf("%d\n", num_garbage);

	fclose(input);
}
