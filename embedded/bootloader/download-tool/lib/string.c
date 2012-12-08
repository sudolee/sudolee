#include "type.h"

int strcmp(const char * str1, const char * str2)
{
	char c1, c2;

	do {
		c1 = *str1++;
		c2 = *str2++;
		if (c1 == '\0')
			return c1 - c2;
	} while (c1 == c2);

	return c1 - c2;
}

char *strcpy(char * dest, char * src)
{
	char c;
	char *s = src;
	const long off = dest - s - 1;

	do {
		c = *s++;
		s[off] = c;
	} while (c != '\0');

	return dest;
}

/**    
 * memcpy - Copy one area of memory to another
 * @dest: Where to copy to    
 * @src: Where to copy from   
 * @count: The size of the area.
 *
 * You should not use this function to access IO space.
 *
 * TODO:
 *   when compile with -Os, this func is requested ?
 */
void *memcpy(void *dest, const void *src, long count)
{
	char *tmp = dest;
	const char *s = src;

	while (count--)
		*tmp++ = *s++;
	return dest;
}

void *memset(void *s, int c, size_t count)
{
	char *s8 = (char *)s;

	while(count--)
		*s8++ = c;

	return s;
}
