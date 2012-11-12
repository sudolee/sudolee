#include <stdio.h>

/* bugs: only Hex and Dec support */
char *myitoa(int num, int base)
{
	int i, flag = num;
	static char buf[13] = {0};
	char ch[16] = "0123456789abcdef";

	if(base == 10)
	{
		for(i = 13; i && num; i--, num /= 10)
			buf[i] = ch[num % 10];	
	}

	if(base == 16)
	{
		for(i = 13; i && num; i--, num /= 16)
			buf[i] = ch[num % 16];
		buf[i--] = 'x';
		buf[i] = '0';
		return &buf[i];
	}

	return &buf[i + 1];
}

int main(void)
{
	printf("%s\n", myitoa(256, 10));

	return 0;
}
