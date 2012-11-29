#ifndef __PRINT_H_
#define __PRINT_H_

#include "_stdarg.h"

long serial_printf(int port_num, const char *format, ...);
long tiny_vsnprintf(char *buf, u32 size, const char *fmt, va_list args);

#endif /* __PRINT_H_ */
