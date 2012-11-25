#ifndef ___STDARG_H__
#define ___STDARG_H__

/* type definitions */
typedef char *va_list;
/* macros */
#define _Bnd(X, bnd) ((sizeof(X) + (bnd)) & ~(bnd))
#define va_arg(ap, T) \
		(*(T *)(((ap) += _Bnd(T, 3U)) - _Bnd(T, 3U)))
#define va_start(ap, A) \
		(void)((ap) = (char *)&(A) + _Bnd(A, 3U))
#define va_end(ap) (void)0

#endif /* ___STDARG_H__ */
