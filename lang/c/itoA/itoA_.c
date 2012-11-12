#include <stdio.h>

char *itoA_(int tmp, int base)
{
	int i, res;
	static char buf[13];

	if(!(base == 10 || base == 16))
		return ">Sorry, only hex and dec supported.";

	buf[12] = '\0';
	i = 11;

	do{
		res = tmp % base;
		tmp /= base;
		buf[i] = "0123456789ABCDEF"[res];
		i--;
	}while(i >= 0 && tmp != 0);

	if(base == 16){
		buf[i--] = 'x';
		buf[i] = '0';
	}

	return &buf[i];
}

int main(void)
{
	printf("%s\n", itoA_(128, 16));

	return 0;
}
