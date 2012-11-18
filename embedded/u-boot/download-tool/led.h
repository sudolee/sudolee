#ifndef _LED_H_
#define _LED_H_

#include "common.h"

extern void led_init(void);

/* 0 <= index <= 3 */
extern void led_run(u32 index);

#endif
