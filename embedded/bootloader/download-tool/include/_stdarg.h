#ifndef ___STDARG_H__
#define ___STDARG_H__

#ifndef _VALIST
#define _VALIST
typedef char *va_list;
#endif              /* _VALIST */

/*
 * Storage alignment properties
 *  32-bits or 64-bits machine.
 */
#define  _AUPBND                (sizeof (long int) - 1)
#define  _ADNBND                (sizeof (long int) - 1)

/*
 * Variable argument list macro definitions
 */
#define _bnd(X, bnd)            (((sizeof (X)) + (bnd)) & (~(bnd)))
#define va_arg(ap, T)           (*(T *)(((ap) += (_bnd (T, _AUPBND))) - (_bnd (T,_ADNBND))))
#define va_end(ap)              (void) 0
#define va_start(ap, A)         (void) ((ap) = (((char *) &(A)) + (_bnd (A,_AUPBND))))


#endif /* ___STDARG_H__ */
