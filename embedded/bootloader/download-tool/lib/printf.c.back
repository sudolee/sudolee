#include "type.h"
#include "print.h"
#include "uart.h"

/* Needed when link */
extern void *memcpy(void *dest, const void *src, long count);

#define SIGN_NUM	(1 << 0)
#define ZERO_PAD	(1 << 1)
#define SMALL_CASE	0x20

#define ADDCH(__str, __ch) 	do{ \
			*(__str) = (__ch); \
			++(__str); \
		} while (0)

/* we use this so that we can do without the ctype library */
#define is_digit(c) (((c) >= '0') && ((c) <= '9'))

static inline int skip_atoi(const char **s)
{
	int i = 0;

	while (is_digit(**s))
		i = i * 10 + *((*s)++) - '0';

	return i;
}

static inline char *string(char *buf, char *end, char *s)
{
	if (s == NULL)
		s = "NULL";

	/* the last one for '\0'  */
	while (!((*s == '\0') || ((buf + 1) > end)))
		ADDCH(buf, *s++);

	return buf;
}

static inline char *number(char *buf, char *end, unsigned long long num, int base,
		  int field_width, int type)
{
	const char digits[] = "0123456789ABCDEF";

	char temp[24];
	int need_prefix = (base != 10);
	int i;
	char locase;

	/* locase = 0 or 0x20. ORing digits or letters with 'locase'
	 * produces same digits or (maybe lowercased) letters */
	locase = (type & SMALL_CASE);

	i = 0;
	if (num == 0) {
		temp[i++] = '0';
	} else if (base != 10) {	/* 8 or 16 */
		int mask = base - 1;
#if 0
		int shift = 3;

		if (base == 16)
			shift = 4;
#endif

		/* it's equal to:
		 *      this = (num & 0xff) % base | locase;
		 *      temp[i] = digits[this];
		 *      num /= base;
		 */
		do {
			temp[i++] = (digits[((unsigned char)num) & mask] | locase);
			num >>= 4;
		} while (num);
	}
#if 0
	else {			/* base == 10, not supported so far */
		/*
		 * http://homepage.cs.uiowa.edu/~jones/bcd/decimal.html
		 */
		do {
			temp[i++] = "0123456789"[num % base];
			num /= base;
		} while (num);
	}
#endif

	/* "-" prefix */
	if (type & SIGN_NUM) {
		if ((long long)num < 0) {
			ADDCH(buf, '-');
			num = -(long long)num;
		}
	}

	/* "0x" / "0" prefix */
	if (need_prefix) {
		ADDCH(buf, '0');
		if (base == 16) {
			ADDCH(buf, ('x' | locase));
		}
	}

	/* even more zero padding? */
	while (i <= --field_width)
		ADDCH(buf, '0');

	/* actual digits of result */
	while (--i >= 0)
		ADDCH(buf, temp[i]);

	return buf;
}

//long vsnprintf(char *buf, u32 size, const char *fmt, va_list args)
long vsnprintf(char *buf, u32 size, const char *fmt, va_list args)
{
	unsigned long long num;
	int base, flags;
	int field_width;
	char *end, *str;

	str = buf;
	end = buf + size;

	for (; *fmt; ++fmt) {
		if (*fmt != '%') {
			ADDCH(str, *fmt);
			continue;
		}

		++fmt;
		/* process flags */
		flags = 0;
		/* TODO: other flags */

		/* get field width */
		field_width = -1;
		if (is_digit(*fmt))
			field_width = skip_atoi(&fmt);

		/* default base */
		base = 16;

		switch (*fmt) {
		case 'c':
			ADDCH(str, (unsigned char)va_arg(args, char));
			continue;

		case 's':
			str = string(str, end, va_arg(args, char *));
			continue;

		case 'p':
			/* flags |= SMALL_CASE */
			str =
			    number(str, end,
				 (unsigned long)va_arg(args, void *), 16,
				 sizeof(void *) << 1, SMALL_CASE);
			continue;

		case '%':
			ADDCH(str, '%');
			continue;

#if 0
		case 'o':
			base = 8;
			break;
#endif

		case 'x':
			flags |= SMALL_CASE;
		case 'X':
			base = 16;
			break;

			/* base = 10 not supported so far
			 * All convert to HEX
			 */
		case 'd':
		case 'i':
			flags |= SIGN_NUM;
		case 'u':
			break;

		default:
			ADDCH(str, '%');
			if (*fmt)
				ADDCH(str, *fmt);
			else
				--fmt;
			continue;
		}

		num = va_arg(args, unsigned int);
		if (flags & SIGN_NUM)
			num = (signed int)num;

		str = number(str, end, num, base, field_width, flags);
	}

	if (size > 0)
		*str = '\0';

	return str - buf;
}

/* TODO: size seem little */
#define MAX_PRINTBUF_SIZE 80

long serial_printf(int port_num, const char *format, ...)
{
	va_list args;
	long rv;
	char printbuffer[MAX_PRINTBUF_SIZE];

	va_start(args, format);
	rv = vsnprintf(printbuffer, sizeof(printbuffer), format, args);
	va_end(args);

	puts(port_num, printbuffer);

	return rv;
}
