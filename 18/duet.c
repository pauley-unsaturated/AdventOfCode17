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


/* Need to write a circular queue 
 * for communication between processes
 */
typedef int64_t word;

#define QUEUE_SIZE 1024
typedef struct queue {
	word buf[QUEUE_SIZE];
	word* begin;
	word* end;
	size_t num_recv;
} queue;

void queue_init(queue* q) {
	bzero(q, sizeof(queue));
	q->begin = q->end = q->buf;
}

size_t queue_count(queue* q) {
	if (q->begin <= q->end) {
		return q->end - q->begin;
	}
	else {
		return ((q->buf + QUEUE_SIZE) - q->begin) + (q->end - q->buf);
	}
}

bool queue_enqueue(queue* q, word val) {
	if (queue_count(q) == QUEUE_SIZE - 1) {
		return false;
	}
	*q->end = val;
	q->end++;
	if (q->end - q->buf >= QUEUE_SIZE) {
		q->end = q->buf;
	}
	q->num_recv++;
	return true;
}

bool queue_is_empty(queue* q) {
	return (q->begin == q->end);
}

bool queue_dequeue(queue* q, word* oVal) {
	if (queue_is_empty(q)) {
		return false;
	}
	if(oVal) {
		*oVal = *q->begin;
	}
	q->begin++;
	if (q->begin - q->buf >= QUEUE_SIZE) {
		q->begin = q->buf;
	}
	return true;
}

typedef enum run_state {
	CAN_RUN, RUNNING, WAIT_ON_RECV, WAIT_ON_SEND, DEAD
} run_state;

/* State */
#define NUM_REGISTERS 26
/* I'm going to assume that the register names are a-z, case insensitive */
typedef struct state {
	run_state run_state;
	struct instr* cur_instr;
	queue* out_queue;
	word registers[NUM_REGISTERS];
	queue in_queue;
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

typedef enum {
	CONTINUE,
	YIELD,
	HALT
} coroutine_state;

coroutine_state do_next_instr(state* cur_state) {
	if(cur_state->cur_instr < instructions ||
		 (cur_state->cur_instr - instructions) >= num_instructions ) {
		cur_state->run_state = DEAD;
		return HALT;
	}
	instr* cur = cur_state->cur_instr;
	word scratch = 0;
	//printf("%lu: %s\n", cur - instructions, name_for_op(cur->opcode));
	
	switch(cur->opcode) {
		
	case SND:
		scratch = get_value(cur_state, &cur->arg1);
		if(!queue_enqueue(cur_state->out_queue, scratch)) {
			/* Yield and let the other process drain its queue */
			cur_state->run_state = WAIT_ON_SEND;
			return YIELD;
		}
		printf("SND %lld\n", scratch);
		break;

	case RCV:
		if (!queue_dequeue(&(cur_state->in_queue), &scratch)) {
			/* Yield and wait for the other process to feed this queue */
			cur_state->run_state = WAIT_ON_RECV;
			return YIELD;
		}
		printf("RCV %lld\n", scratch);
		*get_register(cur_state, &cur->arg1) = scratch;
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
		if (get_value(cur_state, &cur->arg1) > 0LL) {
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
	state processes[2];
	bzero(&processes, sizeof processes);
	for (int i = 0; i < 2; i++) {
		processes[i].cur_instr = instructions;
		processes[i].registers['p' - 'a'] = i;
		queue_init(&processes[i].in_queue);
		processes[i].out_queue = &processes[(i + 1) % 2].in_queue;
		processes[i].run_state = CAN_RUN;
	}

	int proc_num = 0;
	coroutine_state result = CONTINUE;
	
	for (;;) {
		state* process = &processes[proc_num];
		switch (process->run_state) {
		case WAIT_ON_RECV:
			if (queue_is_empty(&(process->in_queue))) {
				// we have a problem: deadlock
				printf("DEADLOCK! (receive)\n");
				goto deadlock;
			}
			else {
				process->run_state = CAN_RUN;
			}
			break;
		case WAIT_ON_SEND:
			if (queue_count(process->out_queue) == (QUEUE_SIZE - 1)) {
				printf("DEADLOCK! (send)\n");
				abort();
			}
			else {
				process->run_state = CAN_RUN;
			}
			break;
		default:
			break;
		}

		while(process->run_state == CAN_RUN || process->run_state == RUNNING) {
			process->run_state = RUNNING;
			printf("%d: %ld\n", proc_num, process->cur_instr - instructions);
			result = do_next_instr(process);
		}
		printf("%d: YIELD (%d)\n", proc_num, process->run_state);
		proc_num = (proc_num + 1) % 2;
	}

 deadlock:
	for (int i = 0; i < 2; i++) {
		printf("%d sent %ld\n", (i + 1) % 2, processes[i].in_queue.num_recv);
	}
	
	return 0;
}
