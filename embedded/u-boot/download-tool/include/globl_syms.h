#ifndef _GLOBL_SYMS_H_
#define _GLOBL_SYMS_H_

#include "misc.h"

/*
 * memory
 */
void mem_init(void);

/*
 * clock power
 */
void cpm_init(void);

/*
 * IRQ
 */

/* Clear I bit in cpsr */
#define __enable_irq() { \
	__asm__ __volatile__("mrs r3, cpsr\n\t" \
		   				 "bic r3, r3, #0x8\n\t" \
						 "msr cpsr, r3":::"r3");}

/* Set I bit in cpsr */
#define __disable_irq() { \
	__asm__ __volatile__("mrs r3, cpsr\n\t" \
		   				 "orr r3, r3, #0x8\n\t" \
						 "msr cpsr, r3":::"r3");}

/*
 * leds for debug
 */
extern void led_init(void);
/* 0 <= index <= 3 */
extern void led_run(u32 index);

/*
 * misc
 */
void delay(u32);

#endif /* _GLOBL_SYMS_H_ */
