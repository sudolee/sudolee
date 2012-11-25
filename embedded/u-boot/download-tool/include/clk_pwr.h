#ifndef __CLK_PWR_H_
#define __CLK_PWR_H_

#include "type.h"
#include "io.h"
#include "s3c2440.h"

/* clock and power controller entry */
#define CLOCK_POWER_ENTRY 0x4c000000

inline void delay(u32 loops);
void clk_pwr_init(void);

#endif /* __CLK_PWR_H_ */
