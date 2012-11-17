#ifndef __CPM_H__
#define __CPM_H__

/*
 *	Clock power management.
 */

#include "common.h"

/* start at 0x4c000000 */
#define CLOCK_POWER_ENTRY 0x4c000000

struct cpm_t {
	u32 locktime;
	u32 mpllcon;
	u32 upllcon;
	u32 clkcon;
	u32 clkslow;
	u32 clkdivn;
	u32 camdivn;
};

void cpm_init(void);

#endif
