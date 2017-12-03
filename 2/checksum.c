/*
--- Day 2: Corruption Checksum ---

As you walk through the door, a glowing humanoid shape yells in your direction. 
"You there! Your state appears to be idle. Come help us repair the corruption in
 this spreadsheet - if we take another millisecond, we'll have to display an 
 hourglass cursor!"

The spreadsheet consists of rows of apparently-random numbers. To make sure the 
recovery process is on the right track, they need you to calculate the 
spreadsheet's checksum. For each row, determine the difference between the 
largest value and the smallest value; the checksum is the sum of all of these 
differences.

For example, given the following spreadsheet:

5 1 9 5
7 5 3
2 4 6 8
The first row's largest and smallest values are 9 and 1, and their difference is
 8.
The second row's largest and smallest values are 7 and 3, and their difference 
is 4.
The third row's difference is 6.
In this example, the spreadsheet's checksum would be 8 + 4 + 6 = 18.
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>
#include <string.h>

bool get_line_diff(FILE* file, int* oDiff) {
	int min = INT_MAX;
	int max = INT_MIN;
	int cur = 0;

	size_t len = 0;
	char* ptr;
	char buf[4096];
	if ((ptr = fgetln(file, &len)) == NULL) {
		return false;
	}

	memcpy(buf, ptr, len);
	ptr = buf;
	buf[len] = '\0';

	while (sscanf(ptr, "%d", &cur) == 1) {
		printf("%d ", cur);
		if (cur < min) min = cur;
		if (cur > max) max = cur;
		ptr++;
		int num_digits = (int)ceil(log10(cur));
		while(num_digits-- > 0)ptr++;
	}
	
	*oDiff = max - min;
	printf("max: %d, min: %d, diff=%d\n", max, min, *oDiff);

	return true;
}
int main(int argc, const char** argv) {
	int diff = 0;
	long result = 0;

	//FILE* file = fopen("test.txt", "r");
	FILE* file = fopen("spreadsheet.txt", "r");
	while(get_line_diff(file, &diff)) {
		result += diff;
	}
	fclose(file);
	
	printf("%ld\n", result);
	
	return 0;
}
