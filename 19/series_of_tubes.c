/*
 * Advent of Code Day 19
 * See README for details
 */

#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

#define NUM_COLS 256
#define NUM_ROWS 256

char tube_map[NUM_ROWS * NUM_COLS] = {0};
char visited_letters[26] = {0};

int main(int argc, const char** argv) {
	if (argc < 2) {
		fprintf(stderr, "Please specify input file!\n");
		return 1;
	}
	
	FILE* in = fopen(argv[1], "r");
	if (!in) {
		fprintf(stderr, "Error opening input file %s\n", argv[1]);
		return 1;
	}

	/* read the map */
	int rows = 0;
	for (int i = 0; i < NUM_ROWS; i++) {
		for (int j = 0; j < NUM_COLS; j++) {
			char c = fgetc(in);
			if (c == EOF) goto done_reading;
			if (c == '\n') break;
			tube_map[j + i * NUM_COLS] = c;
		}
		rows++;
	}
 done_reading:
	
	printf("Read %d rows\n", rows);
	
	int x = 0;
	int y = 0;
	/* find the entry point */
	for (int j = 0; j < NUM_COLS; j++) {
		if (tube_map[j] == '|') break;
		x++;
	}

	int x_dir = 0;
	int y_dir = 1;
	char* end = visited_letters;
	size_t num_steps = 1;
	
	for(;;) {
		x += x_dir;
		y += y_dir;
		num_steps++;
		char c = tube_map[x + y * NUM_COLS];

		if (c == '+') {
			char dirs[] = {1,0, 0,1, -1,0, 0,-1};
			bool found_exit = false;
			for (int dir = 0; dir < 4 && !found_exit; dir++) {
				int t_x_dir = dirs[2*dir];
				int t_y_dir = dirs[2*dir + 1];
				int t_x = x + t_x_dir;
				int t_y = y + t_y_dir;

				if (t_x < 0 || t_y < 0 || t_x >= NUM_COLS || t_y >= NUM_ROWS
						|| (t_y_dir == -y_dir && t_x_dir == -x_dir)) continue;
				char d = tube_map[t_x + t_y * NUM_COLS];
				if ((t_y_dir != 0 && d == '|')
						|| (t_x_dir != 0 && d == '-')) {
					found_exit = true;
				}
				else if (isalpha(d) && isupper(d)) {
					*end++ = d;
					found_exit = true;
				}
					
				if (found_exit) {
					x_dir = t_x_dir;
					x = t_x;
					y_dir = t_y_dir;
					y = t_y;
					num_steps++;
				}
				
			}
			if (!found_exit) goto end;
		}
		else if (isalpha(c) && isupper(c)) {
			*end++ = c;
			int t_x = (x + 2 * x_dir);
			int t_y = (y + 2 * y_dir);
			char expect = '|';
			if (y_dir == 0) {
				expect = '-';
			}
			if (tube_map[t_x + t_y * NUM_COLS] != expect) {
				printf("Found end: %c\n", c);
				goto end;
			}
		}
	}

 end:
	printf("%s\n", visited_letters);
	printf("%zu steps\n", num_steps);
	fclose(in);
	return 0;
}
