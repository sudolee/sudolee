#ifndef __PRINT_H_
#define __PRINT_H_

#include "_stdarg.h"


long printf(const char *format, ...);
long vsnprintf(char *buf, u32 size, const char *fmt, va_list args);



#endif /* __PRINT_H_ */
