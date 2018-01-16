/*
 * Advent of Code Day 23
 * See README for details
 */

#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

typedef int64_t word;

/* State */
#define MAX_REGISTER 'h'
#define MIN_REGISTER 'a'
#define NUM_REGISTERS ((MAX_REGISTER - MIN_REGISTER) + 1)

typedef struct state {
	struct instr* cur_instr;
	word registers[NUM_REGISTERS];
	uint64_t num_muls;
} state;

/* Values */
typedef struct value {
	enum { T_CONST, T_REG } type;
	union {
		char reg;
		word const_val;
	};
} value;

static inline word get_value(state* state, value* val) {
	switch(val->type) {
	case T_CONST:
		return val->const_val;
	case T_REG:
		return state->registers[val->reg - 'a'];
	}
	return 0xffffffffdeadbeefLL;
}

static inline word* get_register(state* state, value* val) {
	assert(val->type == T_REG);
	assert(val->reg >= MIN_REGISTER);
	assert(val->reg <= MIN_REGISTER + NUM_REGISTERS);
	return &(state->registers[val->reg - 'a']);
}

/* Operations */
typedef enum op {
	SET = 0,
	SUB,
	MUL,
	MOD,
	JNZ,
	ILL
} op;

const char* op_names[ILL] = {
	[SET] = "set",
	[SUB] = "sub",
	[MUL] = "mul",
	[MOD] = "mod",
	[JNZ] = "jnz",
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
		/* Dyadic */
	case SET:
	case SUB:
	case MUL:
	case MOD:
	case JNZ:
		cur = parse_arg(cur, &(oInstr->arg2));
		break;
		/* Unpossible */
	case ILL:
		abort();
	}

	return true;
}

bool do_next_instr(state* cur_state) {
	instr* cur = cur_state->cur_instr;
	word scratch = 0;
	if (cur_state->cur_instr < instructions ||
			cur_state->cur_instr >= instructions + num_instructions) {
		// out of bounds!
		return false;
	}
	//printf("%lu: %s\n", cur - instructions, name_for_op(cur->opcode));
	
	switch(cur->opcode) {
		
	case SET:
		*get_register(cur_state, &cur->arg1) = get_value(cur_state, &cur->arg2);
		break;

	case SUB:
		scratch = get_value(cur_state, &cur->arg1);
		scratch -= get_value(cur_state, &cur->arg2);
		*get_register(cur_state, &cur->arg1) = scratch;
		break;
		
	case MUL:
		scratch = get_value(cur_state, &cur->arg1);
		scratch *= get_value(cur_state, &cur->arg2);
		*get_register(cur_state, &cur->arg1) = scratch;
		cur_state->num_muls++;
		break;

	case MOD:
		scratch = get_value(cur_state, &cur->arg1);
		scratch %= get_value(cur_state, &cur->arg2);
		*get_register(cur_state, &cur->arg1) = scratch;
		break;
				
	case JNZ:
		if (get_value(cur_state, &cur->arg1) != 0LL) {
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
	state processes[1];
	bzero(&processes, sizeof processes);
	for (int i = 0; i < (int)(sizeof processes / sizeof processes[0]); i++) {
		processes[i].cur_instr = instructions;
	}

	int proc_num = 0;
	for (;;) {
		state* process;
		bool result;
		process = &processes[proc_num];
		result = do_next_instr(process);
		if (!result) {
			printf("Done!\n");
			break;
		}
	}

	printf("Part 1: %llu multiplies\n", processes[0].num_muls);
	word result = processes[0].registers[MAX_REGISTER - MIN_REGISTER];
	printf("Part 2: 'h' = %llu\n", result);
	return 0;
}
