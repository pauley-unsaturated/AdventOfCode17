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

char output[NUM_PROGRAMS+1];

typedef struct {
	long unsigned hash;
	long unsigned result_hash;
	char input[NUM_PROGRAMS+1];
	char result[NUM_PROGRAMS+1];
} Mem;

#define MEMORY_SIZE 4096
Mem memory[MEMORY_SIZE];

#define wrap(i,len) (((i)>=0)?((i) % (len)):((len)-((-(i))%(len))))

/* Commands */
#define SPIN 's'
#define EXCHANGE 'x'
#define PARTNER 'p'

typedef struct {
	enum { spin, exchange, partner } type;
	union {
		int spin_val;
		struct { int a; int b; } exchange_val;
		struct { char a; char b; } partner_val;
	};
} Command;

#define NUM_COMMANDS 12000
Command commands[NUM_COMMANDS];
size_t num_commands = 0;

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

static inline int find_idx(char name) {
	int offset = prog_ptr - programs;
	for( int i = 0; i < NUM_PROGRAMS; i++) {
		if (programs[i] == name) {
			if (offset <= i) return i - offset;
			return i + (NUM_PROGRAMS - offset);
		}
	}
	return -1;
}

static inline void do_spin(int num) {
	assert(num < NUM_PROGRAMS);
	assert(num >= 0);
	int offset = prog_ptr - programs;
	int abs_spin = offset - num;
	int wrapped = wrap(abs_spin, NUM_PROGRAMS);
	prog_ptr = programs + wrapped;
	assert(prog_ptr >= programs);
	assert(prog_ptr < programs + NUM_PROGRAMS);
}

static inline void do_exchange(int a, int b) {
	assert(a >= 0 && a < NUM_PROGRAMS);
	assert(b >= 0 && b < NUM_PROGRAMS);
	int offset = (int)(prog_ptr - programs);
	int offset_a = wrap(a + offset, NUM_PROGRAMS);
	int offset_b = wrap(b + offset, NUM_PROGRAMS);
	char temp = programs[offset_a];
	programs[offset_a] = programs[offset_b];
	programs[offset_b] = temp;
}

static inline void do_partner(char a, char b) {
	do_exchange(find_idx(a), find_idx(b));
}

void do_command(const Command* command) {
	switch(command->type) {
	case spin:
		do_spin(command->spin_val);
		break;
	case exchange:
		do_exchange(command->exchange_val.a, command->exchange_val.b);
		break;
	case partner:
		do_partner(command->partner_val.a, command->partner_val.b);
		break;
	}
}

// Turns out that hash is a pretty useful
// function
unsigned long hash(const char *str) {
	unsigned long hash = 5381;
	int c;
	
	while (*str) {
		c = *str++;
		/* hash * 33 + c */
		hash = ((hash << 5) + hash) + c; 
	}
	
	return hash;
}

Mem* find_mem(char* input, unsigned long in_hash) {
	if (!in_hash) {
		in_hash = hash(input);
	}
	Mem* result = &memory[in_hash % MEMORY_SIZE];
	while(result->hash && result->hash != in_hash) {
		result++;
		if (result - memory > MEMORY_SIZE)result = memory;
	}
	if (result->hash == in_hash) {
		return result;
	}
	return NULL;
}

Mem* insert_mem(char* input, char* output) {
	unsigned long in_hash = hash(input);
	unsigned long out_hash = hash(output);

	Mem* result = &memory[in_hash % MEMORY_SIZE];
	while(result->hash) {
		result++;
		if (result - memory > MEMORY_SIZE)result = memory;
	}

	result->hash = in_hash;
	result->result_hash = out_hash;
	strcpy(result->input, input);
	strcpy(result->result, output);
	return result;
}

void print_programs(FILE* out) {
	int offset = (int)(prog_ptr - programs);
	for (int i = 0; i < NUM_PROGRAMS; i++) {
		putc(programs[wrap(i + offset, NUM_PROGRAMS)], out);
	}
}

void render_programs(char outBuf[NUM_PROGRAMS+1]) {
	int offset = prog_ptr - programs;
	for (int i = 0; i < NUM_PROGRAMS; i++) {
		outBuf[i] = programs[(i + offset) % NUM_PROGRAMS];
	}
	outBuf[NUM_PROGRAMS] = '\0';
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
			int spin_val = parse_spin(in);
			assert(spin_val >= 0);
			commands[num_commands++] = (Command){
				.type = spin, .spin_val = spin_val
			};
		}
			break;

		case EXCHANGE: {
			int a, b;
			parse_exchange(in, &a, &b);
			commands[num_commands++] = (Command){
				.type = exchange, .exchange_val = { a, b }
			};
		}
			break;

		case PARTNER: {
			char a, b;
			parse_partner(in, &a, &b);
			commands[num_commands++] = (Command){
				.type = partner, .partner_val = { a, b }
			};
		}
			break;

		case ',':
			/* ignore */
		continue;

		default:
			fprintf(stderr, "Unexpected char %c\n", c);
			return -1;
		}
	}
	printf("Read %zu commands\n", num_commands);
	
	for (size_t i = 0; i < num_commands; i++) {
		do_command(commands + i);
	}
	printf("Part 1: ");
	print_programs(stdout);
	printf("\n");

	printf("Part 2: ");
	
	render_programs(output);
	/*
	 * Please excuse the mess, I had to hastily write a memoization
	 * scheme without a proper hash table :-/
	 */
	bzero(memory, sizeof(Mem) * MEMORY_SIZE);
	Mem* mem = insert_mem("abcdefghijklmnop", output);
	for (size_t j = 1; j < 1000000000UL; j++) {
		assert(mem);
		Mem* next = find_mem(mem->result, mem->result_hash);
		if (!next) {
			strcpy(programs, mem->result);
			prog_ptr = programs;
			for (size_t i = 0; i < num_commands; i++) {
				do_command(commands + i);
			}
			render_programs(output);
			next = insert_mem(mem->result, output);
			/*
			printf("%s(%lu) -> %s(%lu)\n", next->input, next->hash,
						 next->result, next->result_hash);
			*/
		}
		assert(next);
		mem = next;
		
		if ((j % 1000000) == 0) {
			putc('.', stdout);
			fflush(stdout);
		}
	}
	strcpy(programs, mem->result);
	prog_ptr = programs;
	
	printf("\n");
	print_programs(stdout);
	printf("\n");
	
	return 0;
}
