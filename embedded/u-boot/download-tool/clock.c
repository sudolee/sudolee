#include "clock.h"

static inline void delay(u32 loops)
{
	__asm__ __volatile__("0:	\n\t"
						 "subs %0, %1, #1	\n\t"
						 "nop	\n\t"
						 "bne 0b":"=r"(loops):"0"(loops));
}

static inline struct clock_power_t *get_clock_power_base(void)
{
	return (struct clock_power_t *)CLOCK_POWER_ENTRY;
}

void clock_power_init()
{
	struct clock_power_t *clock_power = get_clock_power_base();

	clock_power->locktime = 0xffffffff;		/* max pll lock time */

	/* upll clock = 48 MHz, MDIV-PDIV-SDIV */
	clock_power->upllcon = (0x38 << 12) | (0x2 << 4) | (0x2 << 0);

	mb();
	delay(7);

	/* mpll clock = 532 MHz, MDIV-PDIV-SDIV */
	clock_power->mpllcon = (0x7d << 12) | (0x1 << 4) | (0x1 << 0);

	mb();
	delay(7);

	/*
	 * Uclk = UPLL, Hclk = Fclk/2, Pclk = Hclk/2
	 * Fclk:Hclk:Pclk = 1:2:4;
	 */
	clock_power->clkdivn = (0x1 << 3) | (0x1 << 1) | (0x1 << 0);

	mb();
	delay(7);
}
