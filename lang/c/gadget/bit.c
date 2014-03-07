#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Check a bit in bitmap:
 * @ arr: bitmap
 * @ bit: the bit to set
 */
static inline unsigned char check_bit(unsigned char *arr, unsigned short bit)
{
	return (*(arr + (bit >> 3)) & (0x1 << (bit % 8))) ? 1 : 0;
}

/*
 * Set a bit in bitmap
 */
static inline void set_bit(unsigned char *arr, unsigned short bit)
{
	*(arr + (bit >> 3)) |= 0x1 << (bit % 8);
}

/*
 * Clear a bit in bitmap
 */
static inline void clear_bit(unsigned char *arr, unsigned short bit)
{
	*(arr + (bit >> 3)) &= ~(0x1 << (bit % 8));
}

/*
 * Set range of bit in bitmap:
 * @ arr   : bitmap
 * @ Start : the bit to start counting from
 * @ End   : the bit to end   counting
 */
static inline void set_bit_range(unsigned char *arr, unsigned short Start, unsigned short End)
{
	do {
		set_bit(arr, Start++);
	} while (Start <= End);
}

/*
 * Clear range of bit in bitmap
 */
static inline void clear_bit_range(unsigned char *arr, unsigned short Start, unsigned short End)
{
	do {
		clear_bit(arr, Start++);
	} while (Start <= End);
}

static void bits_dump(unsigned char *arr, size_t size)
{
	int i;

	for(i = (size << 3) - 1; i >= 0; i--) {
		printf("%u", check_bit(arr, i));
		if(!(i % 8)) {
			putchar(' ');
		}
		if(!(i % 32)) {
			putchar('\n');
		}
	}
	putchar('\n');
}

/*
 * return -1 when error detect.
 */
int get_range(unsigned char *src, unsigned short *range)
{
	int i;
	char *str, *token;

	if(!src) {
		return -1;
	}

	range[0] = -1;

	/* 0-65535 only need 2 tokens. */
	for (i = 0, str = src; i < 2; i++, str = NULL) {
		token = strtok(str, "-");
		if(token == NULL) {
			break;
		} else {
			range[i] = atoi(token);
		}
	}

	return range[0];
}

int main(int argc, char **argv)
{
	if(argc < 2) {
		printf("Pls enter bit number {0-255} to set/clear/check.\n");
		exit(EXIT_FAILURE);
	} else {
		printf("Bit [%s] will be set and clear.\n", argv[1]);
	}

	unsigned char  arr[8] = {0};
	unsigned short range[2] = {0};

	if(-1 == get_range(argv[1], range)) {
		printf("Oops, invalid range, :(\n");
		exit(EXIT_FAILURE);
	}

	set_bit_range(arr, range[0], range[1]);
	bits_dump(arr, sizeof(arr));

	clear_bit_range(arr, range[0], range[1]);
	bits_dump(arr, sizeof(arr));

	exit(EXIT_SUCCESS);
}
