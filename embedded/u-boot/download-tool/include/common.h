#ifndef __COMMON_H_
#define __COMMON_H_

#include "type.h"


/* based on 2.45ns */
extern inline void ndelay(u32 loops)
{
	__asm__ __volatile__("0:\n\t"
			     "subs %0, %1, #1\n\t"
			     "nop\n\t" "bne 0b":"=r"(loops):"0"(loops));
}

extern inline void udelay(u32 loops)
{
	while(loops--)
		ndelay(400); // about 1us
}

#endif
