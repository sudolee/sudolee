#ifndef __TYPE_H_
#define __TYPE_H_

#define NULL ((void *)0)
#define true (1)
#define false (0)
#define bool (u8)

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned long size_t;

#define offsetof(TYPE, MEMBER) ((u32) &((TYPE *)0)->MEMBER)
#define container_of(ptr, type, member) ({          \
		            const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
		            (type *)( (char *)__mptr - offsetof(type,member) );})

#endif /* __TYPE_H_ */
