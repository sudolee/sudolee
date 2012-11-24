#include "timer.h"

/*
 * Watch dog timer
 */
static inline struct watchdog_t *get_watchdog_base(void)
{
	return (struct watchdog_t *)WATCH_DOG_ENTRY;
}

void soc_reset()
{
	struct watchdog_t *wd = get_watchdog_base();

	/* set watch dog timer data register */
	writel(&wd->wtdat, DEAULT_WTDAT);

	/* set watch dog timer data register */
	writel(&wd->wtcnt, DEAULT_WTCNT);

	/* watchdog timer control register
	 * value = 0b0101 0101 0010 0001
	 * [15:8] = 0b01010101, prescaler value.
	 * [7:6]
	 * [5] = 0b1, enable watchdog timer
	 * [4:3] = 0b00, clk division factor
	 * [2] = 0b0, disable interrupt
	 * [1]
	 * [0] = 0b1, enable reset signal
	 * T = 1/[Pclk/(prescaler value + 1)/division] = 10.7ns
	 */
	writel(&wd->wtcon, WTCON);

	while(1);
}
