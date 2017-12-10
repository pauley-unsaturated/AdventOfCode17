/*
 * Day 10 solution see README for details
 */

#include <stdio.h>

int list[256];
size_t list_len = (sizeof list) / (sizeof list[0]);

int lengths[] = {157,222,1,2,177,254,0,228,159,140,249,187,255,51,76,30};
size_t lengths_len = (sizeof lengths) / (sizeof lengths[0]);

#define circ_index(idx, len) ( (idx) % (len) )

int main(int argc, const char** argv) {

	int cur_idx = 0;
	int cur_skip = 0;


	for (size_t i = 0; i < list_len; i++) list[i] = i;
	
	for (size_t i = 0; i < lengths_len; i++) {
		int length = lengths[i];
		for (int j = 0; j < length/2; j++) {
			int tmp = list[circ_index(cur_idx + j, list_len)];
			list[circ_index(cur_idx + j, list_len)] =
				list[circ_index(cur_idx + (length - 1) - j, list_len)];
			list[circ_index(cur_idx + (length - 1) - j, list_len)] = tmp;
		}
		cur_idx = circ_index(cur_idx + length + cur_skip, list_len);
		cur_skip++;
	}

	printf("%d\n", list[0] * list[1]);
	
	return 0;
}
