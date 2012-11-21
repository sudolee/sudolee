#ifndef __S3C2440_H_
#define __S3C2440_H_

#include "misc.h"

/*
 * Clock power controller
 */

struct cpm_t {
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
struct mem_t {
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

/* Bit map: srcpnd, intpnd, intmod, intoffset, intmsk */
enum {
	EINT0 = 0,
	EINT1 = 1,
	EINT2 = 2,
	EINT3 = 3,
	EINT4_7 = 4,
	EINT8_23 = 5,
	INT_CAM = 6, /* cam_c, cam_p */
	nBATT_FLT = 7,
	INT_TICK = 8,
	INT_WDT_AC97 = 9,
	INT_TIMER0 = 10,
	INT_TIMER1 = 11,
	INT_TIMER2 = 12,
	INT_TIMER3 = 13,
	INT_TIMER4 = 14,
	INT_UART2 = 15, /* rxd2, txd2, err2 */
	INT_LCD = 16,
	INT_DMA0 = 17,
	INT_DMA1 = 18,
	INT_DMA2 = 19,
	INT_DMA3 = 20,
	INT_SDI = 21,
	INT_SPI0 = 22,
	INT_UART1 = 23, /* rxd1, txd1, err1 */
	INT_NFCON = 24,
	INT_USBD = 25,
	INT_USBH = 26,
	INT_IIC = 27,
	INT_UART0 = 28, /* rxd0, txd0, err0 */
	INT_SPI1 = 29,
	INT_RTC = 30,
	INT_ADC = 31, /* adc_s, tc */
};

/* Bit map: subsrcpnd, intsubmsk */
enum {
	/* [31:15] reserved */

	/* uart0 */
	INT_RXD0 = 0,
	INT_TXD0 = 1,
	INT_ERR0 = 2,
	/* uart1 */
	INT_RXD1 = 3,
	INT_TXD1 = 4,
	INT_ERR1 = 5,
	/* uart2 */
	INT_RXD2 = 6,
	INT_TXD2 = 7,
	INT_ERR2 = 8,

	INT_TC = 9,
	INT_ADC_S = 10,
	INT_CAM_C = 11,
	INT_CAM_P = 12,
	INT_WDT = 13,
	INTAC97 = 14,
};



#endif /* __S3C2440_H_ */
