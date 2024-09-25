#include <stdio.h>

int main(){

}

int bad_int_size_is_32(){
	int set_msb = 1 << 31;

	int beyond_msb = 1 << 32;

	return set_msb && !beyond_msb;
}
