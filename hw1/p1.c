#include <stdio.h>

int main(){
	unsigned int x, y, res;

	printf("Enter a num: \n");
	scanf("%x", &x);

	printf("Enter a num: \n");
	scanf("%x", &y);

	res = (y & 0xFFFFFF00) | (x & 0xFF);

	printf("Result: 0x%08X\n", res);

}
