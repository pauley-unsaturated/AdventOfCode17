
/*
--- Part Two ---

As a stress test on the system, the programs here clear the grid and then store 
the value 1 in square 1. Then, in the same allocation order as shown above, they
 store the sum of the values in all adjacent squares, including diagonals.

So, the first few squares' values are chosen as follows:

Square 1 starts with the value 1.
Square 2 has only one adjacent filled square (with value 1), so it also stores 
1.
Square 3 has both of the above squares as neighbors and stores the sum of their 
values, 2.
Square 4 has all three of the aforementioned squares as neighbors and stores the
 sum of their values, 4.
Square 5 only has the first and fourth squares as neighbors, so it gets the 
value 5.
Once a square is written, its value does not change. Therefore, the first few 
squares would receive the following values:

147  142  133  122   59
304    5    4    2   57
330   10    1    1   54
351   11   23   25   26
362  747  806--->   ...

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main(int argc, const char** argv) {
	int target = 265149;
	//int dim = 515; // ceiling of the sqrt of target
	int dim = 515;

	int *buf = (int*)malloc(sizeof(int) * dim * dim);
	bzero(buf, sizeof(int) * dim * dim);
	
	int x, y;

	x = y = (int)ceil(dim / 2);
	int xvel = 0, yvel = 1;

	int num = 1;
	int val = 1;
	while (num <= dim) {
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < num; j++) {
				if(x < dim && y < dim)
					buf[x + dim*y] = val++;
				x+=xvel;
				y+=yvel;
			}
			/*
			for (int i = 0; i < dim; i++) {
				for (int j = 0; j < dim; j++) {
					printf("%02d ", buf[i + dim*j]);
				}

				printf("\n");
			}
			printf("\n");
			*/
			int old_yvel = yvel;
			yvel = xvel;
			xvel = -old_yvel;
		}
		num++;
	}
	
	for (int i = 0; i < dim; i++) {
		for (int j = 0; j < dim; j++) {
			printf("%02d ", buf[i + dim*j]);
		}
		printf("\n");
	}

	return 0;
}
