/*
 * Day 10 solution see README for details
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SPARSE_LEN 256
#define DENSE_LEN 16
#define SPARSE_BYTES_PER_DENSE_BYTE (SPARSE_LEN / DENSE_LEN)

int list[SPARSE_LEN];
const size_t list_len = SPARSE_LEN;

char* input = "157,222,1,2,177,254,0,228,159,140,249,187,255,51,76,30";
//char* input = "AoC 2017";

char closing_sequence[5] = {17, 31, 73, 47, 23};

#define NUM_ROUNDS 64

int cur_idx = 0;
int cur_skip = 0;

#define circ_index(idx, len) ( (idx) % (len) )

void hash_round(const char* input_str, size_t input_str_len) {
	for (size_t i = 0; i < input_str_len; i++) {
		int length = input_str[i];
		for (int j = 0; j < length/2; j++) {
			int tmp = list[circ_index(cur_idx + j, list_len)];
			list[circ_index(cur_idx + j, list_len)] =
				list[circ_index(cur_idx + (length - 1) - j, list_len)];
			list[circ_index(cur_idx + (length - 1) - j, list_len)] = tmp;
		}
		cur_idx = circ_index(cur_idx + length + cur_skip, list_len);
		cur_skip++;
	}
}


int main(int argc, const char** argv) {
	unsigned char dense_hash[DENSE_LEN] = {0};
	char out_str[DENSE_LEN*8 + 1] = {0};
	
	for (size_t i = 0; i < list_len; i++) list[i] = i;
	size_t input_len = strlen(input);

	char *input_buf = (char*)malloc(input_len + sizeof closing_sequence + 1);
	strcpy(input_buf, input);
	memcpy(input_buf + input_len, closing_sequence, sizeof closing_sequence);
	input_len += sizeof closing_sequence;

	for (int i = 0; i < NUM_ROUNDS; i++) hash_round(input_buf, input_len);

	free(input_buf);
	input_buf = NULL;
	
	for (int i = 0; i < DENSE_LEN; i++) {
		for (int j = 0; j < SPARSE_BYTES_PER_DENSE_BYTE; j++) {
			dense_hash[i] ^= list[i * SPARSE_BYTES_PER_DENSE_BYTE + j];
		}
	}
	
	sprintf(out_str,
					"%02x%02x%02x%02x%02x%02x%02x%02x"
					"%02x%02x%02x%02x%02x%02x%02x%02x",
					dense_hash[0],dense_hash[1],dense_hash[2],dense_hash[3],
					dense_hash[4],dense_hash[5],dense_hash[6],dense_hash[7],
					dense_hash[8],dense_hash[9],dense_hash[10],dense_hash[11],
					dense_hash[12],dense_hash[13],dense_hash[14],dense_hash[15]);

	printf("%s\n", out_str);
	
	return 0;
}
