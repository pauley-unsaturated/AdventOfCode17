/* Advent of Code: Day 21
 * See README for details
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>


#define TWO_PATTERN_SIZE 4   // 2 ^ 2
#define THREE_PATTERN_SIZE 9 // 3 ^ 2
#define FOUR_PATTERN_SIZE 16 // 4 ^ 2

typedef bool pixel;

#define PIXEL_OFF false
#define PIXEL_ON  true

/* Two by two patterns */
/* 2^4 patterns max */
#define MAX_TWO_PATTERNS 16 * 4
size_t num_two_patterns = 0;
pixel two_patterns[MAX_TWO_PATTERNS * TWO_PATTERN_SIZE];
pixel two_patterns_output[MAX_TWO_PATTERNS * THREE_PATTERN_SIZE];


/* Three by three patterns */
/* 2^9 patterns pax */
#define MAX_THREE_PATTERNS 512 * 4
size_t num_three_patterns = 0;
pixel three_patterns[MAX_THREE_PATTERNS * THREE_PATTERN_SIZE];
pixel three_patterns_output[MAX_THREE_PATTERNS * FOUR_PATTERN_SIZE];

static const char* initial_pattern = ".#./..#/###";

const char* decode_pattern(const char* pattern, pixel* oBuf, int* dim) {
	*dim = 0;
	bool first_line = true;
	const char* cur = pattern;
	char c;
	while((c = *cur++) != '\0') {
		pixel val = PIXEL_OFF;
		switch (c) {
		case '/':
			first_line = false;
			continue;
		case '.':
			val = PIXEL_OFF;
			break;			
		case '#':
			val = PIXEL_ON;
			break;
		default:
			goto done;
		}		
		*oBuf++ = val;
		if (first_line) (*dim)++;
	}

 done:
	return cur;
}

void display_buf(FILE* out, pixel* buf, int dim) {
	for (int i = 0; i < dim; i++) {
		for (int j = 0; j < dim; j++) {
			pixel val = *buf++;
			fputc(val?'#':'.', out);
		}
		fputc('\n', out);
	}
}

void rotate_clockwise(pixel* buf, unsigned dim) {
	pixel scratch[dim * dim];

	for (unsigned i = 0; i < dim; i++) {
		for (unsigned j = 0; j < dim; j++) {
			scratch[(j * dim) + (dim - 1) - i] = buf[j + i * dim];
		}
	}
	memcpy(buf, scratch, dim * dim);
}

void flip_vertical(pixel* buf, unsigned dim) {
	for (unsigned i = 0; i < dim/2; i++) {
		for (unsigned j = 0; j < dim; j++) {
			pixel temp = buf[j + i * dim];
			buf[j + i * dim] = buf[j + (dim - 1 - i) * dim];
			buf[j + (dim - 1 - i) * dim] = temp;
		}
	}
}

int new_dim(int dim) {
	if (dim % 2) {
		/* 3 -> 4 */
		return 4 * dim / 3;
	}
	else {
		/* 2 -> 3 */
		return 3 * dim / 2;
	}
}

int main(int argc, const char** argv) {
	FILE* in;
	if (argc > 1) {
		in = fopen(argv[1], "r");
	}
	else {
		in = stdin;
	}

	char* line;
	size_t line_len = 0;

	while(NULL != (line = fgetln(in, &line_len))) {
		line[line_len - 1] = '\0';

		pixel match_buf[FOUR_PATTERN_SIZE] = {0};
		pixel output_buf[FOUR_PATTERN_SIZE] = {0};
		int match_dim = 0;
		int output_dim = 0;
		const char* rest;
		rest = decode_pattern(line, match_buf, &match_dim);

		//printf("Read match of dim %d\n", match_dim);

		char c = *rest;
		while(c != '.'  && c != '#') c = *(++rest);

		decode_pattern(rest, output_buf, &output_dim);
		//printf("Read output of dim %d\n", output_dim);

		/* 
		 * Input all rotations and flips into database
		 */
		pixel* match_dest;
		pixel* output_dest;
		size_t match_size;
		size_t output_size;
		size_t* match_idx;
		if (match_dim == 2 && output_dim == 3) {
			match_dest = two_patterns;
			output_dest = two_patterns_output;
			match_size = TWO_PATTERN_SIZE;
			output_size = THREE_PATTERN_SIZE;
			match_idx = &num_two_patterns;
		}
		else if (match_dim == 3 && output_dim == 4) {
			match_dest = three_patterns;
			output_dest = three_patterns_output;
			match_size = THREE_PATTERN_SIZE;
			output_size = FOUR_PATTERN_SIZE;
			match_idx = &num_three_patterns;
		}
		else {
			fprintf(stderr, "Bad pattern!\n");
			return 1;
		}

		int num_rotations = 0;
		while (num_rotations < 4) {
			//printf("Rotate %d\n", num_rotations);
			//display_buf(stdout, match_buf, match_dim);			
			memcpy(match_dest + (*match_idx * match_size),
						 match_buf, match_size);
			memcpy(output_dest + (*match_idx * output_size),
						 output_buf, output_size);
			(*match_idx)++;			

			
			//printf("Flip Vertical\n");
			flip_vertical(match_buf, match_dim);
			//display_buf(stdout, match_buf, match_dim);
			memcpy(match_dest + (*match_idx * match_size),
						 match_buf, match_size);
			memcpy(output_dest + (*match_idx * output_size),
						 output_buf, output_size);
			(*match_idx)++;
			flip_vertical(match_buf, match_dim);

			rotate_clockwise(match_buf, match_dim);			
			num_rotations++;
		}

	}

	printf("Read %zu 2-patterns, %zu 3-patterns\n",
				 num_two_patterns, num_three_patterns);


	pixel* initial_buf = NULL;
	initial_buf = malloc(sizeof(pixel) * FOUR_PATTERN_SIZE);
	int initial_size = 0;

	decode_pattern(initial_pattern, initial_buf, &initial_size);
	printf("Initial:\n");
	display_buf(stdout, initial_buf, initial_size);
	printf("\n");
	
	pixel* cur_buf = initial_buf;
	pixel* next_buf = NULL;
	int cur_dim = initial_size;
	int next_dim = 0;

	pixel scratch[FOUR_PATTERN_SIZE];
	
	for (int iter = 1; iter <= 5; iter++) {
		next_dim = new_dim(cur_dim);
		next_buf = malloc(next_dim * next_dim * sizeof(pixel));
		bzero(next_buf, sizeof(pixel) * next_dim * next_dim);
		/* Do the replacement into next_buf */
		int match_size;
		int output_size;
		pixel* match_base;
		pixel* output_base;
		size_t num_match;
		if (cur_dim % 2) {
			match_size = 3;
			output_size = 4;
			match_base = three_patterns;
			output_base = three_patterns_output;
			num_match = num_three_patterns;
		}
		else {
			match_size = 2;
			output_size = 3;
			match_base = two_patterns;
			output_base = two_patterns_output;
			num_match = num_two_patterns;
		}

		int num_divisions = cur_dim / match_size;
		for (int i = 0; i < num_divisions; i++) {
			for (int j = 0; j < num_divisions; j++) {
				/* copying the j,i-th sub-pattern here into scratch */
				for (int k = 0; k < match_size; k++) {
					for (int l = 0; l < match_size; l++) {
						scratch[l + k * match_size] = cur_buf[((j * match_size) + l) +
																									((i * match_size + k) * cur_dim)];
					}
				}

				//printf("Matching %d,%d\n", j, i);
				//display_buf(stdout, scratch, match_size);

				bool found = false;
				for (size_t test = 0; test < num_match && !found; test++) {
					if (!memcmp(scratch, match_base + test * (match_size * match_size),
											match_size * match_size * sizeof(pixel))) {
						//printf("Match!\n");
						memcpy(scratch, output_base + test * (output_size * output_size),
									 output_size * output_size * sizeof(pixel));
						//display_buf(stdout, scratch, output_size);
						//printf("\n");
						found = true;
					}
				}
				if (!found) {
					printf("Unable to proceed!\n");
					goto done_2;
				}

				/* copying scratch into the j,i-th sub-pattern */
				for (int k = 0; k < output_size; k++) {
					for (int l = 0; l < output_size; l++) {
						next_buf[((j * output_size) + l) + ((i * output_size + k) * next_dim)] = 
							scratch[l + k * output_size];
					}
				}
				//display_buf(stdout, next_buf, next_dim);
				//printf("***\n");
			}
		}

		free(cur_buf);
		cur_dim = next_dim;
		cur_buf = next_buf;
		next_buf = NULL;
		next_dim = 0;
		printf("%d (%d)\n", iter, cur_dim);
		display_buf(stdout, cur_buf, cur_dim);
		printf("\n");
	}

 done_2:
	;
	int num_on = 0;
	for(int i = 0; i < cur_dim; i++) {
		for (int j = 0; j < cur_dim; j++) {
			if (cur_buf[j + i * cur_dim]) num_on++;
		}
	}
	printf("Part 1: %d\n", num_on);
	
	return 0;
}
