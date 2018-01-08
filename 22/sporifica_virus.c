/* 
 * Advent of Code: Day 22
 * See README for details
 */

#include <stdio.h>
#include <stdbool.h>
#include <strings.h>
#include <errno.h>
#include <assert.h>


#define max_input_dim 25
#define max_output_dim 10 * 1024
#define num_iterations 10000000

/*
#define max_input_dim 3
#define max_output_dim 100000
#define num_iterations 10000000
*/

typedef enum {
	Clean,
	Weakened,
	Infected,
	Flagged
} pixel;

pixel input_buf[max_input_dim * max_input_dim];
int input_dim = 0;

pixel output_buf[max_output_dim * max_output_dim];

void parse_input(FILE* in) {
	/* Parse the input */
	char* line;
	size_t line_len = 0;

	int x_dim = 0;
	int y_dim = 0;
	
	while(NULL != (line = fgetln(in, &line_len))) {
		line[line_len - 1] = '\0';
		for (int i = 0; i < max_input_dim; i++) {
			char c = line[i];
			if (c == '\0') input_buf[y_dim * max_input_dim + i] = Clean;
			if (c == '#') input_buf[y_dim * max_input_dim + i] = Infected;
			if (i + 1 > x_dim) x_dim = i + 1;
		}
		y_dim++;
		if (y_dim >= max_input_dim) break;
	}

	if (x_dim != y_dim) {
		fprintf(stderr, "Input dimensions don't match: x=%d y=%d\n", x_dim, y_dim);
	}
	input_dim = y_dim;	
}

void display_buf(FILE* out, pixel* buf, int dim) {
	for (int i = 0; i < dim; i++) {
		for (int j = 0; j < dim; j++) {
			pixel val = *buf++;
			char c;
			switch(val) {
			case Clean:
				c = '.';
				break;
			case Weakened:
				c = 'W';
				break;
			case Flagged:
				c = 'F';
				break;
			case Infected:
				c = '#';
				break;
			}
			fputc(c, out);
			fputc(' ', out);
		}
		
		fputc('\n', out);
	}
}

int main(int argc, const char** argv) {
	FILE* in;

	if (argc > 1) {
		in = fopen(argv[1], "r");
		if (!in) {
			fprintf(stderr, "Unable to open %s: %s\n", argv[1],
							strerror(errno));
			return 1;
		}
	}
	else {
		in = stdin;
	}

	parse_input(in);
	fclose(in);

	printf("Initial:\n");
	display_buf(stdout, input_buf, max_input_dim);

	/* Setup the output buffer */
	int input_center = input_dim / 2;
	int output_center = max_output_dim / 2;
	int offset = output_center - input_center;

	for (int i = 0; i < max_input_dim; i++) {
		memcpy(output_buf + (max_output_dim  * (offset + i)) + offset ,
					 input_buf + input_dim * i, input_dim * sizeof(pixel));
	}

	/* Apply the rules */
	int position[2] = {max_output_dim/2, max_output_dim/2};
	int velocity[2] = {0,-1};

	unsigned long count = 0;
	unsigned long infections = 0;
	
	while (count < num_iterations) {
		pixel c = output_buf[position[0] + position[1] * max_output_dim];

		int temp;
		pixel new;
		switch (c) {
		case Clean:
			/* Clean turns left */
			temp = velocity[1];
			velocity[1] = -velocity[0];
			velocity[0] = temp;
			new = Weakened;
			break;
		case Weakened:
			/* Weakened doesn't turn */
			infections++;
			new = Infected;
			break;
		case Flagged:
			/* Flagged turns around */
			velocity[0] *= -1;
			velocity[1] *= -1;
			new = Clean;
			break;
		case Infected:
			/* Infected turns right */
			temp = velocity[1];
			velocity[1] = velocity[0];
			velocity[0] = -temp;
			new = Flagged;
			break;
		}

		output_buf[position[0] + position[1] * max_output_dim] = new;
		
		/*
			The virus carrier moves forward one node in the direction it is facing. 
		*/
		position[0] += velocity[0];
		position[1] += velocity[1];

		if (position[0] < 0 || position[0] >= max_output_dim ||
				position[1] < 0 || position[1] >= max_output_dim) {
			fprintf(stderr, "Out of bounds! %d, %d\n", position[0], position[1]);
		}

		count++;
		//printf("\n%lu:\n", count);
		//display_buf(stdout, output_buf, max_output_dim);
	}

	printf("Part 1: %lu infections\n", infections);
	
	return 0;
}
