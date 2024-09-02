#include <stdio.h>
#include <stdbool.h>

int main(){
	int x = 0;

	bool a, b, c, d;	

	a = (x != 0);
	b = (x != ~0);  // ~0 creates a value where all bits are 1
	c = (x & 0xFF) != 0; // Mask except first bit, compare to 0
	d = ((x >> ((sizeof(int) * 8) - 8)) & 0xFF) != 0xFF; // Same thing as before, but shift to the most sig. bytes first.

	// printf("a: %d\n", a);
}
