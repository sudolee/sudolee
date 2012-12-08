#ifndef __PRINT_H_
#define __PRINT_H_

#include "_stdarg.h"

long serial_printf(int port_num, const char *format, ...);
long tiny_vsnprintf(char *buf, u32 size, const char *fmt, va_list args);

#define printf(fmt, args...) \
	serial_printf(0, fmt, ##args)

#endif /* __PRINT_H_ */
