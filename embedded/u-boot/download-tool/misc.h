/**************************************************************
 * type.h
 *
 */
#ifndef __TYPE_H_
#define __TYPE_H_

#define NULL ((void *)0)
#define true (1)
#define false (0)
#define bool (u8)

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

#endif /* __TYPE_H_ */

/**************************************************************
 * io.h
 *
 */
#ifndef __IO_H_
#define __IO_H_

#define mb() __asm__ __volatile__("":::"memory")

/* read functions */
#define _arch_readb(addr) (*(volatile u8  *)(addr))
#define _arch_readw(addr) (*(volatile u16 *)(addr))
#define _arch_readl(addr) (*(volatile u32 *)(addr))

#define readb(addr) ({ u8  __v = _arch_readb(addr); mb(); __v; })
#define readw(addr) ({ u16 __v = _arch_readw(addr); mb(); __v; })
#define readl(addr) ({ u32 __v = _arch_readl(addr); mb(); __v; })

/* write functions */
#define _arch_writeb(addr, val) (*(volatile u8  *)(addr) = (val))
#define _arch_writew(addr, val) (*(volatile u16 *)(addr) = (val))
#define _arch_writel(addr, val) (*(volatile u32 *)(addr) = (val))

#define writeb(addr, val) ({ u8  __v = val; mb(); _arch_writel(addr, __v); __v; })
#define writew(addr, val) ({ u16 __v = val; mb(); _arch_writel(addr, __v); __v; })
#define writel(addr, val) ({ u32 __v = val; mb(); _arch_writel(addr, __v); __v; })

#endif /* __IO_H_ */

/**************************************************************
 * string.h
 *
 */
#ifndef __STRING_H_
#define __STRING_H_

int strcmp(u8 *, u8 *); 
u8 * strcpy(u8 *, u8 *);

#endif /* __STRING_H_ */

/**************************************************************
 * misc.h
 *
 */
#ifndef __MISC_H_
#define __MISC_H_

/* useful macros */
#define offsetof(TYPE, MEMBER) ((u32) &((TYPE *)0)->MEMBER)
#define container_of(ptr, type, member) ({          \
		            const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
		            (type *)( (char *)__mptr - offsetof(type,member) );})

#endif	/* __MISC_H_ */
