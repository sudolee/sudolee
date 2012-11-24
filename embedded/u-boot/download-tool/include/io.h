#ifndef __IO_H_
#define __IO_H_

#include "type.h"

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

#define clear_bit(addr, msk) ({ writel((addr), (readl(addr) & ~(msk))); })
#define set_bit(addr, msk) ({ writel((addr), (readl(addr) | (msk))); })

#endif /* __IO_H_ */
