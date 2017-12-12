#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#define NUM_DIRECTIONS 6
typedef enum _Dir {
	N = 0,
	NE,
	SE,
	S,
	SW,
	NW
} Dir;

const char* names[NUM_DIRECTIONS] = {
	"n",
	"ne",
	"se",
	"s",
	"sw",
	"nw"
};

typedef struct _Vector {
	int x;
	int y;
} Vector;

Vector offsets[NUM_DIRECTIONS] = {
	[N]  = { 0,  2},
	[NE] = { 2,  1},
	[SE] = { 2, -1},
	[S]  = { 0, -2},
	[SW] = {-2, -1},
	[NW] = {-2,  1}
};

void do_move(const char* where, Vector* pos) {
	for (int i = 0; i < NUM_DIRECTIONS; i++) {
		if(!strcmp(where, names[i])) {
			Vector *offset = offsets + i;
			pos->x += offset->x;
			pos->y += offset->y;
			return;
		}
	}
}

#define ABS(x) ((x>=0)?(x):(-(x)))

Vector find_next(Vector pos) {
	int distance = ABS(pos.x) + ABS(pos.y);
	Vector result = {0, 0};
	for (int i = 0; i < NUM_DIRECTIONS; i++) {
		Vector offset = offsets[i];
		int new_distance = ABS(pos.x + offset.x) + ABS(pos.y + offset.y);
		if (new_distance < distance) {
			distance = new_distance;
			result = offset;
		}
	}
	return result;
}

int distance(Vector pos) {
	int num_steps = 0;
	while (pos.x != 0 || pos.y != 0) {
		Vector offset = find_next(pos);
		pos.x += offset.x;
		pos.y += offset.y;
		num_steps++;
	}
	return num_steps;
}

int main(int argc, const char** argv) {
	char buf[4] = {0};
	char* cur = buf;
	Vector pos = {0,0};
	int c = 0;
	int max_dist = 0;
	
	while(EOF != (c = getc(stdin))) {
		if (!isspace(c)) {
			if (c == ',') {
				*cur = '\0';
				do_move(buf, &pos);
				cur = buf;
				int dist = distance(pos);
				if (max_dist < dist) {
					max_dist = dist;
				}
			}
			else {
				*cur++ = (char)c;
			}
		}
	}

	if(cur != buf) {
		*cur++ = '\0';
		do_move(buf, &pos);
	}

	printf("(%d, %d)\n", pos.x, pos.y);

	
	printf("%d steps\n", distance(pos));
	printf("%d max steps\n", max_dist);
	
	return 0;
}
