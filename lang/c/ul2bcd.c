/*
 * Convert unsigned long long to bcd code.
 */

#include <stdio.h>

unsigned long long ul2bcd(unsigned long long val)
{
	int i, step;
	unsigned long long new = 0;


	step = (sizeof(val) << 1);
	for(i = 0; i < step; i++) {
		if(new) {
			new >>= 4;
		}
		new |= ((val % 10) << ((step << 2) - 4));
		val /= 10;
	}
	printf("sizeof(val) : %u, %#lx\n", sizeof(val), new);

	return new;
}

int main()
{
	unsigned long long test = 1234567800000000;

	printf("%lu -- bcd --> %#lx\n", test, ul2bcd(test));

	return 0;
}
