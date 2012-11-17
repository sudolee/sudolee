/*
 * Clock power management.
 */

#include "cpm.h"

static inline void delay(u32 loops)
{
	__asm__ __volatile__("0:	\n\t"
						 "subs %0, %1, #1	\n\t"
						 "nop	\n\t"
						 "bne 0b":"=r"(loops):"0"(loops));
}

static inline struct cpm_t *get_cpm_base(void)
{
	return (struct cpm_t *)CLOCK_POWER_ENTRY;
}

void cpm_init(void)
{
	struct cpm_t *clk = get_cpm_base();

	clk->locktime = 0xffffffff;		/* max pll lock time */

	/* upll clock = 48 MHz, MDIV-PDIV-SDIV */
	clk->upllcon = (0x38 << 12) | (0x2 << 4) | (0x2 << 0);

	mb();
	delay(7);

	/* mpll clock = 405 MHz, MDIV-PDIV-SDIV */
	clk->mpllcon = (0x7f << 12) | (0x2 << 4) | (0x1 << 0);

	mb();
	delay(7);

	/*
	 * Uclk = UPLL; Fclk:Hclk:Pclk = 1:3:6;
	 * HDIVN-PDIVN
	 */
	clk->clkdivn = (0x0 << 3) | (0x3 << 1) | (0x1 << 0);

	mb();
	delay(7);
}
