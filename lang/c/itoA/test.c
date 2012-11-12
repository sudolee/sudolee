#include <stdio.h>

/* bugs: if the first argument is zero, itoa cannot work. */
char *itoa(int val, int base)
{
	
	static char buf[11] = {0};
	
	int i = 11;

	for(; val && i ; --i, val /= base)
	
		buf[i] = "0123456789abcdef"[val % base];
	
	return &buf[i + 1];
	
}

int main(void)
{
	printf("%s\n", itoa(128, 16));

	return 0;
}
