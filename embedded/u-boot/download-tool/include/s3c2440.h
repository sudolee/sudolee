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
	u32 intoffset;		/* read only */
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
	u8 txdata;
	u8 pad0[3];
	u8 rxdata;
	u8 pad1[3];
#else
	u8 txdata;
	u8 pad0[3];
	u8 rxdata;
	u8 pad1[3];
#endif
	u32 ubrdiv;
};

/*
 * Watch dog timer
 */
struct watchdog_t {
	u32 wtcon;
	u32 wtdat;
	u32 wtcnt;
};

/*
 * Nand flash
 */
struct nand_ctrl_t {
	u32 nfconf;
	u32 nfcont;
	u32 nfcmmd;		//[7:0]
	u32 nfaddr;		//[7:0]
	u32 nfdata;
	u32 nfmeccd[2];
	u32 nfseccd;
	u32 nfstat;
	u32 nfestat[2];
	u32 nfmecc[2];		/* read only */
	u32 nfsecc;		/* read only */
	u32 nfsblk;
	u32 nfeblk;
};

/*
 * GPIO
 */
/* GPA0 ~ GPA24 */
#define GPACON 0x56000000
#define GPADAT 0x56000004

/* GPH0 ~ GPH10 */
#define GPHCON 0x56000070
#define GPHDAT 0x56000074
#define GPHUP  0x56000078

#endif /* __S3C2440_H_ */
