/* 
 * Advent of code
 * Day 16
 * See README for details
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* State */
#define NUM_PROGRAMS 16
char programs[] = "abcdefghijklmnop";
//#define NUM_PROGRAMS 5
//char programs[] = "abcde";
char* prog_ptr = programs;


/* Commands */
#define SPIN 's'
#define EXCHANGE 'x'
#define PARTNER 'p'

#define wrap(i,len) (((i)>=0)?((i) % (len)):((len)-((-(i))%(len))))

/* Parsers */

int parse_spin(FILE* in) {
	int result;
	if (1 != fscanf(in, "%d", &result)) return -1;
	return result;
}

void parse_exchange(FILE* in, int* a, int* b) {
	fscanf(in, "%d/%d", a, b);
}

void parse_partner(FILE* in, char* a, char* b) {
	fscanf(in, "%c/%c", a, b);
}

int find_idx(char name) {
	int offset = prog_ptr - programs;
	for( int i = 0; i < NUM_PROGRAMS; i++) {
		if (programs[i] == name) {
			if (offset <= i) return i - offset;
			return i + (NUM_PROGRAMS - offset);
		}
	}
	return -1;
}

void do_spin(int num) {
	assert(num < NUM_PROGRAMS);
	assert(num >= 0);
	int offset = prog_ptr - programs;
	int abs_spin = offset - num;
	int wrapped = wrap(abs_spin, NUM_PROGRAMS);
	prog_ptr = programs + wrapped;
	assert(prog_ptr >= programs);
	assert(prog_ptr < programs + NUM_PROGRAMS);
}

void do_exchange(int a, int b) {
	assert(a >= 0 && a < NUM_PROGRAMS);
	assert(b >= 0 && b < NUM_PROGRAMS);
	int offset = (int)(prog_ptr - programs);
	int offset_a = wrap(a + offset, NUM_PROGRAMS);
	int offset_b = wrap(b + offset, NUM_PROGRAMS);
	char temp = programs[offset_a];
	programs[offset_a] = programs[offset_b];
	programs[offset_b] = temp;
}

void do_partner(char a, char b) {
	do_exchange(find_idx(a), find_idx(b));
}

void print_programs(FILE* out) {
	int offset = (int)(prog_ptr - programs);
	for (int i = 0; i < NUM_PROGRAMS; i++) {
		putc(programs[wrap(i + offset, NUM_PROGRAMS)], out);
	}
}

int main(int argc, char** argv) {
	FILE* in;
	if (argc < 2)  {
		in = stdin;
	}
	else {
		in = fopen(argv[1], "r");
	}

	int c;
	print_programs(stdout);
	printf("\n");
	while(EOF != (c = getc(in))) {
		switch(c) {
		case SPIN: {
			int spin = parse_spin(in);
			printf("s%d: ", spin);
			assert(spin >= 0);
			do_spin(spin);
		}
			break;

		case EXCHANGE: {
			int a, b;
			parse_exchange(in, &a, &b);
			printf("x%d/%d: ", a, b);
			do_exchange(a, b);
		}
			break;

		case PARTNER: {
			char a, b;
			parse_partner(in, &a, &b);
			printf("p%c/%c: ", a, b);
			do_partner(a, b);
		}
			break;

		case ',':
			/* ignore */
		continue;

		default:
			fprintf(stderr, "Unexpected char %c\n", c);
			return -1;
		}
		print_programs(stdout);
		printf("\n");
	}
	printf("Part 1: ");
	print_programs(stdout);
	printf("\n");
	
	return 0;
}
