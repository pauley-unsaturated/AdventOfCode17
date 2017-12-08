/*
	--- Day 7: Recursive Circus ---
	
	Wandering further through the circuits of the computer, you come upon a tower of
	programs that have gotten themselves into a bit of trouble. A recursive 
	algorithm has gotten out of hand, and now they're balanced precariously in a 
	large tower.
	
	One program at the bottom supports the entire tower. It's holding a large disc, 
	and on the disc are balanced several more sub-towers. At the bottom of these 
	sub-towers, standing on the bottom disc, are other programs, each holding their 
	own disc, and so on. At the very tops of these sub-sub-sub-...-towers, many 
	programs stand simply keeping the disc below them balanced but with no disc of 
	their own.
	
	You offer to help, but first you need to understand the structure of these 
	towers. You ask each program to yell out their name, their weight, and 
	(if they're holding a disc) the names of the programs immediately above them 
	balancing on that disc. You write this information down (your puzzle input). 
	Unfortunately, in their panic, they don't do this in an orderly fashion; by the 
	time you're done, you're not sure which program gave which information.
	
	For example, if your list is the following:
	
	pbga (66)
	xhth (57)
	ebii (61)
	havc (66)
	ktlj (57)
	fwft (72) -> ktlj, cntj, xhth
	qoyq (66)
	padx (45) -> pbga, havc, qoyq
	tknk (41) -> ugml, padx, fwft
	jptl (61)
	ugml (68) -> gyxo, ebii, jptl
	gyxo (61)
	cntj (57)
	...then you would be able to recreate the structure of the towers that looks 
	like this:
	
                gyxo
              /     
         ugml - ebii
       /      \     
      |         jptl
      |        
      |         pbga
     /        /
tknk --- padx - havc
     \        \
      |         qoyq
      |             
      |         ktlj
       \      /     
         fwft - cntj
              \     
                xhth

  In this example, tknk is at the bottom of the tower (the bottom program), and is
	holding up ugml, padx, and fwft. Those programs are, in turn, holding up other 
	programs; in this example, none of those programs are holding up any other 
	programs, and are all the tops of their own towers. 
	(The actual tower balancing in front of you is much larger.)
	
	Before you're ready to help them, you need to make sure your information is 
	correct. What is the name of the bottom program?
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

typedef struct {
	unsigned long name_hash;
	char name[16];
	int weight;
	int parent_idx;
	int num_children;
	int child_idxs[13];
} Node;

#define NUM_NODES 4096

Node node_table[NUM_NODES] = {0};

int node_table_len = 0;

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

int find_node(const char* name) {
	unsigned long name_hash = hash(name);
	Node* cur = node_table;
	Node* end = node_table + node_table_len;
	while(cur < end) {
		if (cur->name_hash == name_hash) {
			return cur - node_table;
		}
		cur++;
	}
	return -1;
}


bool load_node(FILE* file, Node* oNode) {
	char buf[1024];
	size_t len = 1024;
	char* line = NULL;
	if(NULL == (line = fgetln(file, &len))) {
		return false;
	}
	memcpy(buf, line, len);
	buf[len] = '\0';

	char *c = buf;
	while(*c != ' ') {
		c++;
	}
	*c = '\0';
	
	assert(strlen(buf) < (sizeof oNode->name));
	strcpy(oNode->name, buf);
	oNode->name_hash = hash(oNode->name);
	oNode->num_children = 0;
	oNode->parent_idx = -1;
	
	/* Parse the weight */
	c+=2;
	if (1 != sscanf(c, "%d", &oNode->weight)) {
		fprintf(stderr, "Error parsing node %s\n", buf);
		return false;
	}
	
	return true;
}

void load_nodes() {
	FILE* input = fopen("input.txt", "r");
	
	node_table_len = 0;
	Node* cur_node = node_table;
	while(load_node(input, cur_node)) {
		node_table_len++;
		cur_node++;
	}
	
	fclose(input);
}

bool connect_node(FILE* file, Node* node) {
	char buf[1024];
	size_t len = 1024;
	char* line = NULL;
	if(NULL == (line = fgetln(file, &len))) {
		return false;
	}
	memcpy(buf, line, len);
	buf[len] = '\0';
	
	char* children_start = strstr(buf, " ->");
	if(children_start) {
		children_start += 4;
		/* There are children, parse them */
		while(*children_start) {
			while(isspace(*children_start)) {
				/* Skip whitespace */
				children_start++;
			}
			if (*children_start) {				
				char* child_end = children_start;
				while(*child_end && *child_end != ',' && !isspace(*child_end)) {
					child_end++;
				}
				*child_end = '\0';
				
				int child_idx = find_node(children_start);
				assert(child_idx >= 0);
				assert(child_idx < node_table_len);
				Node* child = &node_table[child_idx];
				assert(child->parent_idx == -1);
				child->parent_idx = node - node_table;
				assert(node->num_children < 13);
				node->child_idxs[node->num_children++] = child_idx;

				children_start = child_end + 1;
			}
		}
	}
	
	return true;
}

void connect_nodes() {
	FILE* input = fopen("input.txt", "r");

	Node* cur_node = node_table;
	int i = 0;
	printf("connecting %p\n", cur_node);
	while(connect_node(input, cur_node)) {
		cur_node++;
		printf("connecting %p\n", cur_node);
	}
	
	fclose(input);
}

int find_root() {
	Node* cur_node = node_table + node_table_len / 2;
	while (cur_node->parent_idx != -1) {
		cur_node = &node_table[cur_node->parent_idx];
	}
	return cur_node - node_table;
}

char* node_name(int node_idx) {
	assert(node_idx >= 0);
	assert(node_idx < node_table_len);
	Node* node = &node_table[node_idx];
	return node->name;
}
	

int main(int argc, const char **argv) {
	printf("Loading nodes...\n");
	load_nodes();
	printf("Loaded %d nodes\n", node_table_len);
	printf("Connecting nodes\n");
	connect_nodes();

	char* root_name = node_name(find_root());
	printf("Root = %s\n", root_name);
	
	return 0;
}
