#ifndef __S3C2440_H_
#define __S3C2440_H_

#include "type.h"
#define LITTLE_ENDIAN

/*
 * Clock power controller
 */

struct clk_pwr_t {
	u32 locktime;
	u32 mpllcon;
	u32 upllcon;
	u32 clkcon;
	u32 clkslow;
	u32 clkdivn;
	u32 camdivn;
};

/*
 * memory controller
 */
struct memory_t {
	u32 bwscon;
	u32 bankcon[8];
	u32 refresh;
	u32 banksize;
	/* Can not be reconfiged during code perform in RAM. */
	u32 mrsrb6;
	u32 mrsrb7;
};

/*
 * interrupts controller
 */
struct irq_regs_t {
	u32 srcpnd;
	u32 intmod;
	u32 intmsk;
	u32 priority;
	u32 intpnd;
	u32 intoffset;	/* read only */
	u32 subsrcpnd;
	u32 intsubmsk;
};

/*
 * uart controller
 */

struct uart_t {
	u32 ulcon;
	u32 ucon;
	u32 ufcon;
	u32 umcon;
	u32 utrstat;
	u32 uerstat;
	u32 ufstat;
	u32 umstat;
#ifdef LITTLE_ENDIAN
	u8 utxh;
	u8 pad0[3];
	u8 urxh;
	u8 pad1[3];
#else
	u8 pad0[3];
	u8 utxh;
	u8 pad1[3];
	u8 urxh;
#endif /* LITTLE_ENDIAN */

	u32 ubrdiv;
};

#endif /* __S3C2440_H_ */
