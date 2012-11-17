#ifndef _MEM_H_
#define _MEM_H_

#include "common.h"

#define MEMORY_ENTRY 0x48000000

struct mem_t {
	u32 bwscon;
	u32 bankcon0;
	u32 bankcon1;
	u32 bankcon2;
	u32 bankcon3;
	u32 bankcon4;
	u32 bankcon5;
	u32 bankcon6;
	u32 bankcon7;
	u32 refresh;
	u32 banksize;

	/* Can not be reconfiged during code perform in RAM. */
	u32 mrsrb6;
	u32 mrsrb7;
};

void mem_init(void);


#endif
