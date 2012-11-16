#ifndef __CLOCK_H__
#define __CLOCK_H__

#include "common.h"

/* start at 0x4c000000 */
#define CLOCK_POWER_ENTRY 0x4c000000

struct clock_power_t {
	u32 locktime;
	u32 mpllcon;
	u32 upllcon;
	u32 clkcon;
	u32 clkslow;
	u32 clkdivn;
	u32 camdivn;
};

void clock_power_init(void);

#endif
