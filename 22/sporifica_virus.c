/* 
 * Advent of Code: Day 22
 * See README for details
 */

#include <stdio.h>
#include <stdbool.h>
#include <strings.h>
#include <errno.h>

#define max_input_dim 25
#define max_output_dim 10 * 1024

typedef bool pixel;
#define INFECTED ((pixel)true)
#define CLEAN (!INFECTED)

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
			if (c == '\0') input_buf[y_dim * max_input_dim + i] = false;
			if (c == '#') input_buf[y_dim * max_input_dim + i] = true;
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
			fputc(val?'#':'.', out);
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
					 input_buf + input_dim * i, input_dim);
	}

	//printf("\n");
	//printf("Initital (with space):\n");
	//display_buf(stdout, output_buf, max_output_dim);


	/* Apply the rules */
	int position[2] = {max_output_dim/2, max_output_dim/2};
	int velocity[2] = {0,-1};

	unsigned long count = 0;
	unsigned long infections = 0;
	
	while (count < 10000) {
		pixel c = output_buf[position[0] + position[1] * max_output_dim];
		/*
			If the current node is infected, it turns to its right.
			Otherwise, it turns to its left.
			(Turning is done in-place; the current node does not change.)
		*/
		if (c == INFECTED) {
			int temp = velocity[1];
			velocity[1] = velocity[0];
			velocity[0] = -temp;
		}
		else {
			int temp = velocity[1];
			velocity[1] = -velocity[0];
			velocity[0] = temp;
			infections++;
		}

		/*
			If the current node is clean, it becomes infected. 
			Otherwise, it becomes cleaned. 
			(This is done after the node is considered for the purposes of changing 
			direction.)
		*/
		if (c == INFECTED) {
			output_buf[position[0] + position[1] * max_output_dim] = CLEAN;
		}
		else {
			output_buf[position[0] + position[1] * max_output_dim] = INFECTED;
		}
		

		/*
			The virus carrier moves forward one node in the direction it is facing. 
		*/
		position[0] += velocity[0];
		position[1] += velocity[1];

		count++;
		//printf("\n%lu:\n", count);
		//display_buf(stdout, output_buf, max_output_dim);
	}

	printf("Part 1: %lu infections\n", infections);
	
	return 0;
}
