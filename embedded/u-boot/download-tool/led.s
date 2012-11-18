.section .text

/* void led_init(void); */
.globl led_init
led_init:
	.align
	.equ GPBCON, 0x56000010		@ Port B control register
	.equ GPBDAT, 0x56000014		@ Port B data register
	ldr r0, =GPBCON			@ get value of GPBCON var
	ldr r1, [r0]			@ get status of GPBCON register
#	bic r1, r1, #0x0003fc03 @ bad immediate
	bic r1, r1, #0x0003fc00 @ clear r1[17:10]
	bic r1, r1, #0x00000003 @ clear r1[1:0]
#	orr	r1, r1, #0x00015401
	orr	r1, r1, #0x00015400
	orr	r1, r1, #0x00000001
	str r1, [r0]			@ set GPB0,5,6,7,8 as output port

/*
 * disable beep
 * GPBDAT bit0, 0enable, 1disable.
 */
	ldr r1, =GPBDAT			@ get value of GPBDAT var
	ldr r2, [r1]
	bic r2, r2, #0x1
	str r2, [r1]			@ disable beep

	mov pc, lr

/*
 * void led_run(u32 index);
 * 0 <= index <=3
 * pass and return via r0.
 *
 * GPB(5~8) connected to nLED_(1~4), 0 enable, 1 disable.
 */
.globl led_run
led_run:
	cmp r0, #3				@ check whether a valid index
	bhi out					@ if index > 3, go out
	ldr r2, =GPBDAT			@ get value of GPBDAT var
	ldr r1, [r2]			@ get status of GPBDAT register
	bic r1, r1, #0x1e0

	add r0, r0, #5			@ shift left at least 5 bits
	mov r3, #1				@ enable port anfter shift
	orr r1, r1, r3, lsl r0	@ set r2 as new status we want to output to GPBDAT
	str r1, [r2]
	
out:
	mov pc, lr

