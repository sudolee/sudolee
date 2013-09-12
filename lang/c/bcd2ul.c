#include <stdio.h>

/*
 * imsi   MAX : 999999999999999 = 0x3 8d7e a4c6 7fff = 52-bits
 * msisdn MAX :   9999999999999 =    0x918 4e72 9fff = 44-bits
 */
struct new_imsi_msisdn_ethhd {
	union {
		struct {
			unsigned long long imsi:52; unsigned long long msisdn_a:12;
			unsigned int msisdn_b;
		};
		unsigned int data[3];
	};
}__attribute__((packed));

/* convert bcd to unsigned 64 */
static unsigned long long bcd2ul(unsigned long long val)
{
	unsigned int i, step, count;
	unsigned long long new;

	/* 16 */
	new = 0;
	step = sizeof(val) << 1;
	for(i = 0, count = step; i < step; i++) {
		if(new) {
			new *= 10;
		}
		new += ((val >> ((count << 2) - 4)) & 0xF);
		val &= (0x0FFFFFFFFFFFFFFF >> (i << 2));
		count--;
	}
	
	return new;
}

int main()
{
	printf("%#lx -- dec --> %Lu\n", 0x1357924680, bcd2ul(0x1357924680));

	return 0;
}
