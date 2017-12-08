/*
--- Day 8: I Heard You Like Registers ---

You receive a signal directly from the CPU. Because of your recent assistance 
with jump instructions, it would like you to compute the result of a series of 
unusual register instructions.

Each instruction consists of several parts: the register to modify, whether to 
increase or decrease that register's value, the amount by which to increase or 
decrease it, and a condition. If the condition fails, skip the instruction 
without modifying the register. The registers all start at 0. The instructions 
look like this:

b inc 5 if a > 1
a inc 1 if b < 5
c dec -10 if a >= 1
c inc -20 if c == 10
These instructions would be processed as follows:

Because a starts at 0, it is not greater than 1, and so b is not modified.
a is increased by 1 (to 1) because b is less than 5 (it is 0).
c is decreased by -10 (to 10) because a is now greater than or equal to 1 
(it is 1).
c is increased by -20 (to -10) because c is equal to 10.
After this process, the largest value in any register is 1.

You might also encounter <= (less than or equal to) or != (not equal to). 
However, the CPU doesn't have the bandwidth to tell you what all the registers 
are named, and leaves that to you to determine.

What is the largest value in any register after completing the instructions in 
your puzzle input?
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

typedef struct _Register {
	unsigned long hash;
	char name[16];
	long value;
} Register;

typedef enum _Operation {
	INC = 1,
	DEC = -1
} Operation;

typedef enum _Condition {
	LT, // <
	LE, // <=
	EQ, // ==
	GE, // >=
	GT, // >
	NE  // !=
} Condition;

typedef struct _Predicate {
	Register* subject;
	Condition cond;
	long constant;
} Predicate;

typedef struct _Instruction {
	Register* target;
	Operation op;
	long constant;
	Predicate predicate;
} Instruction;

#define MAX_REGISTERS 4096
Register registers[MAX_REGISTERS] = {0};

#define MAX_INSTRUCTIONS 4096
Instruction instructions[MAX_INSTRUCTIONS] = {0};

// register utils
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

Register* find_or_create_register(char* name) {
	assert(name);
	unsigned long name_hash = hash(name);
	Register* cur = registers;
  Register* end = registers + MAX_REGISTERS;
	while(cur != end) {
		if (cur->hash == name_hash) {
			return cur;
		}
		else if (cur->hash == 0) {
			// This is the new register
			cur->hash = name_hash;
			assert(strlen(name) < (16));
			strcpy(cur->name, name);
			cur->value = 0;
			return cur;
		}
		cur++;
	}
	return NULL;
}
	
bool my_getline(FILE* file, char* buf, size_t count) {
	char* line = NULL;
	size_t len = count;
	if(NULL == (line = fgetln(file, &len))) {
		return false;
	}
	memcpy(buf, line, len);
	buf[len] = '\0';
	return true;
}

bool load_instruction(char* line, Instruction* instruction) {
	char* cursor = line;
	while(*cursor && !isspace(*cursor))cursor++;
	*cursor = '\0';
	instruction->target = find_or_create_register(line);
	line = ++cursor;
	
	while (*cursor && !isspace(*cursor))cursor++;
	*cursor = '\0';
	if (!strcmp("inc", line)) {
		instruction->op = INC;
	}
	else {
		instruction->op = DEC;
	}
	line = ++cursor;
	
	while (*cursor && !isspace(*cursor))cursor++;
	*cursor = '\0';
	instruction->constant = atol(line);
	line = ++cursor;

	while (*cursor && !isspace(*cursor))cursor++;
	*cursor = '\0';
	assert(!strcmp("if", line));
	line = ++cursor;

	while (*cursor && !isspace(*cursor))cursor++;
	*cursor = '\0';
	instruction->predicate.subject = find_or_create_register(line);
	line = ++cursor;

	while (*cursor && !isspace(*cursor))cursor++;
	*cursor = '\0';
	if     (!strcmp("<",  line)) instruction->predicate.cond = LT;
	else if(!strcmp("<=", line)) instruction->predicate.cond = LE;
	else if(!strcmp("==", line)) instruction->predicate.cond = EQ;
	else if(!strcmp(">=", line)) instruction->predicate.cond = GE;
	else if(!strcmp(">",  line)) instruction->predicate.cond = GT;
	else if(!strcmp("!=", line)) instruction->predicate.cond = NE;
	line = ++cursor;

	while (*cursor && !isspace(*cursor))cursor++;
	*cursor = '\0';
	instruction->predicate.constant = atol(line);
	
	return true;
}

bool check_predicate(Predicate* predicate) {
	switch(predicate->cond) {
	case LT:
		return predicate->subject->value < predicate->constant;
		break;
	case LE:
		return predicate->subject->value <= predicate->constant;
		break;
	case EQ:
		return predicate->subject->value == predicate->constant;
		break;
	case GE:
		return predicate->subject->value >= predicate->constant;
		break;
	case GT:
		return predicate->subject->value > predicate->constant;
		break;
	case NE:
		return predicate->subject->value != predicate->constant;
		break;
	}
	return false;
}

void do_instruction(Instruction* instruction) {
	// Check the predicate
	if (check_predicate(&instruction->predicate)) {
		// Update the target
		instruction->target->value += ((int)instruction->op) * instruction->constant;
	}
}

// Initialization
void load_instructions(FILE* file) {
	char buf[1024];
	Instruction* cur = instructions;

	while(my_getline(file, buf, sizeof buf)) {
		if(load_instruction(buf, cur)) {
			cur++;
		}
	}
	printf("Read %ld instructions\n", cur - instructions);
	
}

void dump_registers() {
	Register* r = registers;
	while(r->hash) {
		printf("%s(%ld) ", r->name, r->value);
		r++;
	}
	printf("\n");
}

int main(int argc, const char** argv) {
	FILE* input = fopen("input.txt", "r");
	
	load_instructions(input);
	// could probably do the instructions during the load
	Instruction* cur = instructions;
	while(cur->target) {
		do_instruction(cur);
		cur++;
	}

	dump_registers();

	Register* r = registers;
	long int max_val = r->value;
	while(r->hash) {
		if (r->value > max_val) {
			max_val = r->value;
		}
		r++;
	}

	printf("%ld\n", max_val);
	
	fclose(input);
}
