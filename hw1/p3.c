#include <stdio.h>

int any_odd_one(unsigned x);


int main(){
	
	// printf("Hello World! \n");
	
	unsigned x = 0x4;

	printf("Result: %d\n", any_odd_one(x));
	
	unsigned y = 0x2;

	printf("Result: %d\n", any_odd_one(y));

	return 0;
}

int any_odd_one(unsigned x){
	unsigned odd_bit_mask = 0xAAAAAAAA;

	return (x & odd_bit_mask) != 0;
}
