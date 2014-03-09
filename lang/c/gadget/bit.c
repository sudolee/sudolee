/*
 * Note:
 *  Bit 0 is the LSB of addr; bit 32 is the LSB of (addr+1).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Check a bit in bitmap:
 * @ arr: bitmap
 * @ bit: the bit to set
 */
static inline unsigned int check_bit(unsigned int *arr, int bit)
{
	return (*(arr + (bit >> 5)) & (1U << (bit % 32))) != 0;
}

/*
 * Set a bit in bitmap
 */
static inline void set_bit(unsigned int *arr, int bit)
{
	unsigned int  mask = 1U << (bit % 32);
	unsigned int *pos  = arr + (bit >> 5);

	*pos |= mask;
}

/*
 * Clear a bit in bitmap
 */
static inline void clear_bit(unsigned int *arr, int bit)
{
	unsigned int  mask = 1U << (bit % 32);
	unsigned int *pos  = arr + (bit >> 5);

	*pos &= ~mask;
}

/*
 * Set range of bit in bitmap:
 * @ arr   : bitmap
 * @ Start : the bit to start counting from
 * @ End   : the bit to end   counting
 */
static inline void set_bit_range(unsigned int *arr, int Start, int End)
{
	do {
		set_bit(arr, Start++);
	} while (Start <= End);
}

/*
 * Clear range of bit in bitmap
 */
static inline void clear_bit_range(unsigned int *arr, int Start, int End)
{
	do {
		clear_bit(arr, Start++);
	} while (Start <= End);
}

static void bits_dump(unsigned int *arr, size_t size)
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
int get_range(char *src, int *range)
{
	int i;
	char *str, *token;

	if(!src) {
		return -1;
	}

	range[0] = -1;

	/* {0-65535} only need 2 tokens. */
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
//	unsigned int arr[0x10000 >> 5] = {0};
	unsigned int arr[8] = {0};
	int max_bit = (sizeof(arr) << 3) - 1;

	if(argc < 2) {
		printf("Pls enter bit number {0-%u} to set/clear/check.\n", max_bit);
		exit(EXIT_FAILURE);
	} else {
		printf("Bit [%s] will be set and clear.\n", argv[1]);
	}

	int range[2] = {0, 0};

	if(-1 == get_range(argv[1], range)) {
		printf("Oops, invalid range, :(\n");
		exit(EXIT_FAILURE);
	} else {
		if((range[0] > max_bit) || (range[1] > max_bit)) {
			printf("Oops, range out of {0-%u}, :(\n", max_bit);
			exit(EXIT_FAILURE);
		}
	}

	printf("Set bit(s)...\n");
	set_bit_range(arr, range[0], range[1]);
	bits_dump(arr, sizeof(arr));

	printf("Then, clear set bit(s)...\n");
	clear_bit_range(arr, range[0], range[1]);
	bits_dump(arr, sizeof(arr));

	exit(EXIT_SUCCESS);
}
