/*
 * Advent of Code, Day 14
 * See README for details
 */

/*
 * Reusing the knot-hash code from day 10..
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define SPARSE_LEN 256
#define DENSE_LEN 16
#define SPARSE_BYTES_PER_DENSE_BYTE (SPARSE_LEN / DENSE_LEN)
#define NUM_ROUNDS 64
int list[SPARSE_LEN];
const size_t list_len = SPARSE_LEN;


//char* input = "flqrgnkx";
char* input = "ffayrhll";
char closing_sequence[5] = {17, 31, 73, 47, 23};

char input_scratch[1024];
#define NUM_ROWS 128


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

void knot_hash(const char* input_str, uint8_t oHash[DENSE_LEN]) {
	// Init the hash.
	for (size_t i = 0; i < list_len; i++) list[i] = i;
	cur_idx = 0;
	cur_skip = 0;
	
	size_t input_len = strlen(input_str);
	char input_buf[input_len + (sizeof closing_sequence) + 1];
	memcpy(input_buf, input_str, input_len);
	memcpy(input_buf + input_len, closing_sequence, sizeof closing_sequence);
	input_len += (sizeof closing_sequence);
	input_buf[(sizeof input_buf) - 1] = '\0';

	// Perform 64 rounds of the hash
	for (int i = 0; i < NUM_ROUNDS; i++) hash_round(input_buf, input_len);
	
	for (int i = 0; i < DENSE_LEN; i++) {
		oHash[i] = 0;
		for (int j = 0; j < SPARSE_BYTES_PER_DENSE_BYTE; j++) {
			oHash[i] ^= list[i * SPARSE_BYTES_PER_DENSE_BYTE + j];
		}
	}
}

unsigned int num_bits_on_in_byte(uint8_t byte) {
	unsigned int result = 0U;
	for(int i = 0; i < 8; i++) {
		result += (byte >> i) & (uint8_t)0x01;
	}
	return result;
}

long unsigned num_bits_on_in_buffer(uint8_t* buf, size_t size) {
	long unsigned result = 0LU;
	uint8_t* end = buf + size;
	do {
		result += num_bits_on_in_byte(*buf);
	} while(++buf != end);
	
	return result;
}

void render_buffer(const uint8_t* hash_field, uint8_t* dest,
									 size_t row_size, size_t num_rows) {
	for (size_t i = 0; i < num_rows; i++) {
		for (size_t j = 0; j < row_size; j++) {
				for (int k = 0; k < 8; k++) {
					if ( *hash_field & (0x80u >> k)) {
						*dest++ = '#';
					}
					else {
						*dest++ = '.';
					}
				}
				hash_field++;
		}
	}
}

typedef struct {
	size_t x;
	size_t y;
} Coord;

void do_flood(uint8_t* blocks, size_t cols, size_t rows,
							Coord where, int group) {
	Coord pending[4096];
	Coord* cur = pending;

	*cur++ = where;

	while(cur != pending) {
		Coord c = *(--cur);
		uint8_t* where = blocks + c.x + (cols * c.y);
		if (*where == '#') {
			*where = '+';
			if (c.x > 0) *cur++ = (Coord){c.x - 1, c.y};
			if (c.y > 0) *cur++ = (Coord){c.x, c.y - 1};
			if (c.x < cols - 1) *cur++ = (Coord){c.x + 1, c.y};
			if (c.y < rows - 1) *cur++ = (Coord){c.x, c.y + 1};
		}
	}
}

void display_buffer(uint8_t* buffer, size_t cols, size_t rows) {
	for (size_t i = 0; i < rows; i++) {
		for (size_t j = 0; j < cols; j++) {
			putchar(buffer[j + i*cols]);
		}
		putchar('\n');
	}
}

int flood_fill(uint8_t* blocks, size_t cols, size_t rows) {
	int result = 0;
	for (size_t i = 0; i < rows; i++) {
		for (size_t j = 0; j < cols; j++) {
			if (blocks[j + (i * cols)] == '#') {
				result++;
				do_flood(blocks, cols, rows, (Coord){j, i}, result);
			}
		}
	}
	return result;
}

int main(int argc, const char** argv) {
	uint8_t hash_field[DENSE_LEN * NUM_ROWS] = {0};

	for (int i = 0; i < NUM_ROWS; i++) {
		sprintf(input_scratch, "%s-%d", input, i);
		knot_hash(input_scratch, &hash_field[i * DENSE_LEN]);
	}

	long unsigned num_taken = num_bits_on_in_buffer(hash_field,
																									DENSE_LEN * NUM_ROWS);
	printf("Part 1: %lu\n", num_taken);

	uint8_t blocks[DENSE_LEN * 8 * NUM_ROWS] = {0};
	render_buffer(hash_field, blocks, DENSE_LEN, NUM_ROWS);

	int num_groups = flood_fill(blocks, DENSE_LEN * 8, NUM_ROWS);
	printf("Part 2: %d\n", num_groups);
	
	return 0;
}
