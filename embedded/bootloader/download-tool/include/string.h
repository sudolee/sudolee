#ifndef __STRING_H_
#define __STRING_H_

int strcmp(const char * str1, const char * str2);
char *strcpy(char * dest, char * src);
void *memcpy(void *dest, const void *src, long count);
void *memset(void *s, int c, size_t count);

#endif /* __STRING_H_ */
