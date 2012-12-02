#ifndef __IO_H_
#define __IO_H_

#include "type.h"

#define mb() __asm__ __volatile__("":::"memory")

/* read functions */
#define __raw_readb(addr) (*(volatile u8  *)(addr))
#define __raw_readw(addr) (*(volatile u16 *)(addr))
#define __raw_readl(addr) (*(volatile u32 *)(addr))

#define readb(addr) ({ u8  __v = __raw_readb(addr); mb(); __v; })
#define readw(addr) ({ u16 __v = __raw_readw(addr); mb(); __v; })
#define readl(addr) ({ u32 __v = __raw_readl(addr); mb(); __v; })

/* write functions */
#define __raw_writeb(addr, val) (*(volatile u8  *)(addr) = (val))
#define __raw_writew(addr, val) (*(volatile u16 *)(addr) = (val))
#define __raw_writel(addr, val) (*(volatile u32 *)(addr) = (val))

#define writeb(addr, val) ({ u8  __v = val; mb(); __raw_writel(addr, __v); __v; })
#define writew(addr, val) ({ u16 __v = val; mb(); __raw_writel(addr, __v); __v; })
#define writel(addr, val) ({ u32 __v = val; mb(); __raw_writel(addr, __v); __v; })

/* sb read functions... */
extern inline void __raw_readsl(u32 addr, char *data, int len)
{
	u32 *buf = (u32 *)data;
	while(len--)
		*buf++ = __raw_readl(addr);
}

extern inline void __raw_readsw(u32 addr, char *data, int len)
{
	u16 *buf = (u16 *)data;
	while(len--)
		*buf++ = __raw_readw(addr);
}

extern inline void __raw_readsb(u32 addr, char *data, int len)
{
	u8 *buf = (u8 *)data;
	while(len--)
		*buf++ = __raw_readb(addr);
}

#define readsb(addr, data, len) __raw_readsb(addr, data, len)
#define readsw(addr, data, len) __raw_readsw(addr, data, len)
#define readsl(addr, data, len) __raw_readsl(addr, data, len)

/* sb write functions... */
extern inline void __raw_writesl(u32 addr, const char *data, int len)
{
	u32 *buf = (u32 *)data;
	while(len--)
		__raw_writel(addr, *buf++);
}

extern inline void __raw_writesw(u32 addr, const char *data, int len)
{
	u16 *buf = (u16 *)data;
	while(len--)
		__raw_writew(addr, *buf++);
}

extern inline void __raw_writesb(u32 addr, const char *data, int len)
{
	u8 *buf = (u8 *)data;
	while(len--)
		__raw_writeb(addr, *buf++);
}

#define writesb(addr, data, len) __raw_writesb(addr, data, len)
#define writesw(addr, data, len) __raw_writesw(addr, data, len)
#define writesl(addr, data, len) __raw_writesl(addr, data, len)

#define clear_bit(addr, msk) ({ writel((addr), (readl(addr) & ~(msk))); })
#define set_bit(addr, msk) ({ writel((addr), (readl(addr) | (msk))); })

#endif /* __IO_H_ */
