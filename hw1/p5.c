#include <stdio.h>
#include <stdbool.h>
#include <limits.h>


int main(){
	int x = INT_MIN;
	int y = 0;

	unsigned ux = (unsigned) x;
	unsigned uy = (unsigned) y;
	
	
	bool a, b, c, d, e;

	a = (x < y) == (-x > -y); // Doesn't work for INT_MIN
	b = ((x + y) << 4) + y - x == 17 * y + 15 * x; // always 1
	c = ~x + ~y + 1 == ~(x + y); // always 1
	d = (ux - uy) == -(unsigned) (y-x); // always 1
	e = ((x >> 2) << 2) <= x; // always 1

	printf("a: %d\n", a);
	printf("b: %d\n", b);
	printf("c: %d\n", c);
	printf("d: %d\n", d);
	printf("e: %d\n", e);
}
