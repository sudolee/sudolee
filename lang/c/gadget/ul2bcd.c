/*
 * Convert unsigned long long to bcd code.
 */

#include <stdio.h>

unsigned long long ul2bcd(unsigned long long dec)
{
	unsigned int i, step;
	unsigned long long bcd = 0;


	step = (sizeof(dec) << 1);
	for(i = 0; i < step; i++) {
		if(bcd) {
			bcd >>= 4;
		}
		bcd |= ((dec % 10) << ((step << 2) - 4));
		dec /= 10;
	}

	return bcd;
}

int main()
{
	unsigned long long test = 1234567800000000;

	printf("%Lu -- bcd --> %#Lx\n", test, ul2bcd(test));

	return 0;
}
