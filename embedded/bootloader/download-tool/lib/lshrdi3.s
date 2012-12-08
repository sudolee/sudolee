
#ifdef __ARMEB__	@ big endian
#define al r1
#define ah r0
#else
#define al r0
#define ah r1
#endif

.globl __lshrdi3
.globl __aeabi_llsr
__lshrdi3:
__aeabi_llsr:

    subs    r3, r2, #32 
    rsb ip, r2, #32 
    movmi   r0, r0, lsr r2
    movpl   r0, r1, lsr r3
    orrmi   r0, r0, r1, lsl ip
    mov r1, r1, lsr r2
    mov pc, lr
