#ifndef __COMMON_H_
#define __COMMON_H_

#define mb() __asm__ __volatile__("":::"memory")
#define NULL ((void *)0)

typedef unsigned long u32;
typedef unsigned char u8;


#endif	/* __COMMON_H_ */
