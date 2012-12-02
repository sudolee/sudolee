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

#define min(x, y) ({                \
		    typeof(x) _min1 = (x);          \
		    typeof(y) _min2 = (y);          \
		    (void) (&_min1 == &_min2);      \
		    _min1 < _min2 ? _min1 : _min2; })

#define max(x, y) ({                \
		    typeof(x) _max1 = (x);          \
		    typeof(y) _max2 = (y);          \
		    (void) (&_max1 == &_max2);      \
		    _max1 > _max2 ? _max1 : _max2; })

/* Error numbers */
#define EINVAL 22 /* invalid argument */
#define EROFS 30 /* read only filesystem */
#define EIO 5 /* I/O error */

#endif /* __COMMON_H_ */
