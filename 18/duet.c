/*
 * Advent of Code: Day 18
 * See README for details
 */

#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

/* I'm going to assume that the register names are a-z, case insensitive */

/* State */
#define NUM_REGISTERS 26
typedef struct state {
	long int registers[NUM_REGISTERS];
	long int cur_sound_freq;
	struct instr* cur_instr;
} state;

/* Values */
typedef struct value {
	enum { T_CONST, T_REG } type;
	union {
		char reg;
		long int const_val;
	};
} value;

static inline long int get_value(state* state, value* val) {
	switch(val->type) {
	case T_CONST:
		return val->const_val;
	case T_REG:
		return state->registers[val->reg - 'a'];
	}
	return 0xffffffffdeadbeefL;
}

static inline long int* get_register(state* state, value* val) {
	assert(val->type == T_REG);
	return &(state->registers[val->reg - 'a']);
}

/* Operations */
typedef enum op {
	SND = 0,
	SET,
	ADD,
	MUL,
	MOD,
	RCV,
	JGZ,
	ILL
} op;

const char* op_names[ILL] = {
	[SND] = "snd",
	[SET] = "set",
	[ADD] = "add",
	[MUL] = "mul",
	[MOD] = "mod",
	[RCV] = "rcv",
	[JGZ] = "jgz",
};

op op_for_name(const char* name) {
	for (int i = 0; i < ILL; i++) {
		if (!strcmp(name, op_names[i])) {
			return (op)i;
		}
	}
	return ILL;
}

const char* name_for_op(op op_code) {
	return op_names[op_code];
}

/* Instructions */
typedef struct instr {
	op    opcode;
	value arg1;
	value arg2;
} instr;

#define MAX_INSTRS 4096
instr instructions[MAX_INSTRS];
long num_instructions;

const char* parse_arg(const char* line, value* val) {
	const char* cur = line;
	if(isdigit(*cur) || *cur == '-') {
		unsigned long const_val = 0;
		bool neg = false;
		if (*cur == '-') {
			neg = true;
			cur++;
		}
		while(isdigit(*cur)) {
			const_val = (const_val * 10) + (*cur - '0');
			cur++;
		}
		val->type = T_CONST;
		val->const_val = const_val;
		if (neg) val->const_val *= -1;
	}
	else {
		val->type = T_REG;
		val->reg = *cur++;
	}
	return cur;
}
							 
bool parse_instruction(const char* line, instr* oInstr) {
	char buf[32];
	const char* cur = line;
	char* cur_buf = buf;
	
	while(isspace(*cur))cur++;
	if(!*cur)return false;
	
	while(!isspace(*cur)) *cur_buf++ = *cur++;
	*cur_buf = '\0';
	oInstr->opcode = op_for_name(buf);
	if (oInstr->opcode == ILL) return false;

	cur++;
	cur = parse_arg(cur, &(oInstr->arg1));
	while(*cur && isspace(*cur))cur++;
	
	switch(oInstr->opcode) {
		/* Monadic */
	case SND:
	case RCV:
		break;
		/* Dyadic */
	case SET:
	case ADD:
	case MUL:
	case MOD:
	case JGZ:
		cur = parse_arg(cur, &(oInstr->arg2));
		break;
		/* Unpossible */
	case ILL:
		abort();
	}
	
	return true;
}

bool do_next_instr(state* cur_state) {
	if(cur_state->cur_instr < instructions ||
		 cur_state->cur_instr - instructions >= num_instructions ) {
		return false;
	}
	instr* cur = cur_state->cur_instr;
	long int scratch = 0;
	//printf("%lu: %s\n", cur - instructions, name_for_op(cur->opcode));
	
	switch(cur->opcode) {
		
	case SND:
		cur_state->cur_sound_freq = get_value(cur_state, &cur->arg1);
		break;
	case RCV:
		if (get_value(cur_state, &cur->arg1) != 0) {
			printf("Playing sound at %ld hz\n", cur_state->cur_sound_freq);
		}
		break;

	case SET:
		*get_register(cur_state, &cur->arg1) = get_value(cur_state, &cur->arg2);
		break;

	case ADD:
		scratch = get_value(cur_state, &cur->arg1);
		scratch += get_value(cur_state, &cur->arg2);
		*get_register(cur_state, &cur->arg1) = scratch;
		break;
		
	case MUL:
		scratch = get_value(cur_state, &cur->arg1);
		scratch *= get_value(cur_state, &cur->arg2);
		*get_register(cur_state, &cur->arg1) = scratch;
		break;
		
	case MOD:
		scratch = get_value(cur_state, &cur->arg1);
		scratch %= get_value(cur_state, &cur->arg2);
		*get_register(cur_state, &cur->arg1) = scratch;
		break;
		
	case JGZ:
		if (get_value(cur_state, &cur->arg1) > 0) {
			cur_state->cur_instr += (get_value(cur_state, &cur->arg2) - 1);
		}
		break;
		/* Unpossible */
	case ILL:
		abort();
	}
	cur_state->cur_instr++;
	return true;
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
	instr* cur_instr = instructions;

	while(NULL != (line = fgetln(in, &line_len))) {
		line[line_len - 1] = '\0';
		parse_instruction(line, cur_instr);
		cur_instr++;
	}
	if (in != stdin) {
		fclose(in);
	}

	num_instructions = cur_instr - instructions;
	
	// Init the state
	state cur_state;
	bzero(&cur_state, sizeof(state));
	cur_state.cur_instr = instructions;

	while(do_next_instr(&cur_state));
	
	return 0;
}
