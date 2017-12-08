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
	int sum_weights;
	int parent_idx;
	int num_children;
	int child_idxs[12];
} Node;

#define NUM_NODES 4096

Node node_table[NUM_NODES] = {0};

int node_table_len = 0;

Node* node_for_idx(int idx) {
	assert(idx >= 0);
	assert(idx < node_table_len);
	return &node_table[idx];
}

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
	oNode->sum_weights = 0;
	
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
				assert(node->num_children < (int)(sizeof node->child_idxs / sizeof(int)));
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
	while(connect_node(input, cur_node)) {
		cur_node++;
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

void calculate_weight(Node* node) {
	node->sum_weights = node->weight;
	for (int i = 0; i < node->num_children; i++) {
		Node* child = &node_table[node->child_idxs[i]];
		calculate_weight(child);
		node->sum_weights += child->sum_weights;
	}	
}

bool node_is_balanced(Node* node) {
	if (node->num_children == 0) return true;
	/* TAKE THE FIRST BORN */
	int first_child = node_for_idx(node->child_idxs[0])->sum_weights;
	for (int i = 1; i < node->num_children; i++) {
		if (first_child != node_for_idx(node->child_idxs[i])->sum_weights)
			return false;
	}
	return true;
}

Node* find_imbalanced(Node* node) {
	/* Assume that at most one of the nodes is different
	 * 
	 */
	if(!node_is_balanced(node)) {
		for (int i = 0; i < node->num_children; i++) {
			Node* child = node_for_idx(node->child_idxs[i]);
			if (!node_is_balanced(child)) return child;
		}
		return node;
	}
	return NULL;
}


void dump_tree(Node* node, int depth) {
	for (int i = 0; i < depth; i++) printf("\t");
	printf("%s (%d):(%d)\n", node->name, node->sum_weights, node->weight);
	for (int i = 0; i < node->num_children; i++) {
		Node* child = node_for_idx(node->child_idxs[i]);
		dump_tree(child, depth + 1);
	}
}

int main(int argc, const char **argv) {
	printf("Loading nodes...\n");
	load_nodes();
	printf("Loaded %d nodes\n", node_table_len);
	printf("Connecting nodes\n");
	connect_nodes();

	Node *root_node = &node_table[find_root()];
	assert(root_node);
	
	char *root_name = root_node->name;
	printf("Root = %s\n", root_name);

	calculate_weight(root_node);

	dump_tree(root_node, 0);
	
	Node* imbalanced = find_imbalanced(root_node);
	printf("Imbalanced = %s\n", imbalanced->name);
	for (int i = 0; i < imbalanced->num_children; i++) {
		Node *child = node_for_idx(imbalanced->child_idxs[i]);
		/* I'm going to cheap out here and do the balancing manually.
		 * This could be automated, but the code would be a bit nasty.
		 * Basically use a frequency list to determine the outlier,
		 * then for the outlier, determine which of it's children needs to have the
		 * weight changed, and by how much (again, by finding the outlier).
		 */
		dump_tree(child, 1);
	}

	
	return 0;
}
