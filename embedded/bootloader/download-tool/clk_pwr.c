/*
 * Clock power management.
 */

#include "clk_pwr.h"
#include "common.h"

inline void clk_pwr_init(void)
{
	struct clk_pwr_t *clk = (struct clk_pwr_t *)CLOCK_POWER_ENTRY;

	writel(&clk->locktime, 0xffffffff);	/* max pll lock time */

	/* upll clock = 48 MHz, MDIV-PDIV-SDIV */
	writel(&clk->upllcon, (0x38 << 12) | (0x2 << 4) | (0x2 << 0));

	ndelay(7);

	/* mpll clock = 405 MHz, MDIV-PDIV-SDIV */
	writel(&clk->mpllcon, (0x7f << 12) | (0x2 << 4) | (0x1 << 0));

	ndelay(7);

	/*
	 * Uclk = UPLL; Fclk:Hclk:Pclk = 1:3:6;
	 * HDIVN-PDIVN
	 */
	writel(&clk->clkdivn, (0x0 << 3) | (0x3 << 1) | (0x1 << 0));

	ndelay(7);
}
