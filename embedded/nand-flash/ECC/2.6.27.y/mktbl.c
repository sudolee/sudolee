#include <stdio.h>

#define BIT0(x) (((x) & 0x1) >> 0)
#define BIT1(x) (((x) & 0x2) >> 1)
#define BIT2(x) (((x) & 0x4) >> 2)
#define BIT3(x) (((x) & 0x8) >> 3)
#define BIT4(x) (((x) & 0x10) >> 4)
#define BIT5(x) (((x) & 0x20) >> 5)
#define BIT6(x) (((x) & 0x40) >> 6)
#define BIT7(x) (((x) & 0x80) >> 7)

static unsigned char ecc_tbl[256];

/*
 * Make the table local in
 * linux-2.6.27.y/drivers/mtd/nand/nand_ecc.c
 */
void mkECCtbl(void)
{
	unsigned char i;
	unsigned cp0, cp1, cp2, cp3, cp4, cp5;
	unsigned rp;

	cp0 = 0, cp1 = 0, cp2 = 0, cp3 = 0, cp4 = 0, cp5 = 0;
	rp = 0;

	/*
	 * cp0 = bit6 ^ bit4 ^ bit2 ^ bit0;
	 * cp1 = bit7 ^ bit5 ^ bit3 ^ bit1;
	 * cp2 = bit5 ^ bit4 ^ bit1 ^ bit0;
	 * cp3 = bit7 ^ bit6 ^ bit3 ^ bit2;
	 * cp4 = bit3 ^ bit2 ^ bit1 ^ bit0;
	 * cp5 = bit7 ^ bit6 ^ bit5 ^ bit4;
	 *
	 * rp = xor all bits;
	 */
	for (i = 0; i < 0xff; i++) {
		unsigned char xdata = 0;

		cp0 = BIT0(i) ^ BIT2(i) ^ BIT4(i) ^ BIT6(i);
		if (cp0)
			xdata |= 0x1;

		cp1 = BIT1(i) ^ BIT3(i) ^ BIT5(i) ^ BIT7(i);
		if (cp1)
			xdata |= 0x2;

		cp2 = BIT0(i) ^ BIT1(i) ^ BIT4(i) ^ BIT5(i);
		if (cp2)
			xdata |= 0x4;

		cp3 = BIT2(i) ^ BIT3(i) ^ BIT6(i) ^ BIT7(i);
		if (cp3)
			xdata |= 0x8;

		cp4 = BIT0(i) ^ BIT1(i) ^ BIT2(i) ^ BIT3(i);
		if (cp4)
			xdata |= 0x10;

		cp5 = BIT4(i) ^ BIT5(i) ^ BIT6(i) ^ BIT7(i);
		if (cp5)
			xdata |= 0x20;

		rp = BIT0(i) ^ BIT1(i) ^ BIT2(i) ^ BIT3(i)
		   ^ BIT4(i) ^ BIT5(i) ^ BIT6(i) ^ BIT7(i);
		if (rp)
			xdata |= 0x40;

		ecc_tbl[i] = xdata;
	}
}

int main()
{
	int i;

	mkECCtbl();

	for (i = 0; i < 0xff;) {
		printf("0x%2.2x, ", ecc_tbl[i]);

		i++;
		if (!(i % 0x10))
			putchar('\n');
	}

	putchar('\n');
	return 0;
}
