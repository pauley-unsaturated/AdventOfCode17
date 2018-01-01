/* Advent of Code: Day 21
 * See README for details
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>



#define TWO_PATTERN_SIZE 4   // 2 ^ 2
#define THREE_PATTERN_SIZE 9 // 3 ^ 2
#define FOUR_PATTERN_SIZE 16 // 4 ^ 2

typedef bool pixel;

#define PIXEL_OFF false
#define PIXEL_ON  true

/* Two by two patterns */
/* 2^4 patterns max */
#define MAX_TWO_PATTERNS 16
size_t num_two_patterns = 0;
pixel two_patterns[MAX_TWO_PATTERNS * TWO_PATTERN_SIZE];
pixel two_patterns_output[MAX_TWO_PATTERNS * THREE_PATTERN_SIZE];


/* Three by three patterns */
/* 2^9 patterns pax */
#define MAX_THREE_PATTERNS 512
size_t num_three_patterns = 0;
pixel three_patterns[MAX_THREE_PATTERNS * THREE_PATTERN_SIZE];
pixel three_paterns_output[MAX_THREE_PATTERNS * FOUR_PATTERN_SIZE];

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

		printf("Read match of dim %d\n", match_dim);

		char c = *rest;
		while(c != '.'  && c != '#') c = *(++rest);

		decode_pattern(rest, output_buf, &output_dim);
		printf("Read output of dim %d\n", output_dim);
		
	}
	
	pixel initial_buf[FOUR_PATTERN_SIZE] = {0};
	int initial_size = 0;

	decode_pattern(initial_pattern, initial_buf, &initial_size);

	printf("Initial:\n");
	display_buf(stdout, initial_buf, initial_size);
	
	return 0;
}
