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
static unsigned long long bcd2ul(unsigned long long bcd)
{
	unsigned int i, step, count;
	unsigned long long dec;

	/* 16 */
	dec = 0;
	step = sizeof(bcd) << 1;
	for(i = 0, count = step; i < step; i++) {
		if(dec) {
			dec *= 10;
		}
		dec += ((bcd >> ((count << 2) - 4)) & 0xF);
		bcd &= (0x0FFFFFFFFFFFFFFF >> (i << 2));
		count--;
	}
	
	return dec;
}

int main()
{
	printf("%#lx -- dec --> %Lu\n", 0x1357924680, bcd2ul(0x1357924680));

	return 0;
}
