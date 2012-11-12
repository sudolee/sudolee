#include <stdio.h>

#define SIZE 12

int main()
{
	char buf[SIZE] = {
		0, 1, 0, 2,
		0, 3, 0, 4,
		0, 5, 0, 6,
	};
	int *point_1;
	char *point_2;
	short *point_3;

	point_1 = (int *)buf;
	point_2 = buf;
	point_3 = (short *)buf;

	printf("*p1 = 0x%08x, *p2 = 0x%08x, *p3 = 0x%08x.\n", *(point_1 + 1), *(point_2 + 1), *(point_3 + 1));
	printf("1[buf] = %d, buf[1] = %d\n", 1[buf], *(buf + 1));

	return 0;
}
