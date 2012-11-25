#include "type.h"

int strcmp(const u8 * str1, const u8 * str2)
{
	u8 c1, c2;

	do {
		c1 = *str1++;
		c2 = *str2++;
		if (c1 == '\0')
			return c1 - c2;
	} while (c1 == c2);

	return c1 - c2;
}

u8 *strcpy(u8 * dest, u8 * src)
{
	u8 c;
	u8 *s = src;
	const long off = dest - s - 1;

	do {
		c = *s++;
		s[off] = c;
	} while (c != '\0');

	return dest;
}
