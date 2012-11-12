/*
 * Little program for downloading binary into nand/nor flash (QT2440).
 * No parallel and serial port. Just with JLink.
 */

.section .text
.globl _start

_start:
	b start_code
	.rept 7					@ For 7 exceptions
		nop
	.endr

start_code:
disable_watchdog:
	.equ WTCON, 0x53000000	@ watchdog timer control register
	.align
	ldr r0, =WTCON			@ get value of WTCON var
	mov r1, #0x0
	str r1, [r0]			@ clear WTCON register

disable_beep:
	.equ GPBCON, 0x56000010		@ Port B control register
	.equ GPBDAT, 0x56000014		@ Port B data register
	.align
	ldr r0, =GPBCON			@ get value of GPBCON var
	ldr r1, [r0]			@ get status of GPBCON register
	bic r1, r1, #0x00000003 @ clear r1[1:0]
	orr	r1, r1, #0x00000001
	str r1, [r0]			@ set GPB0 as output port

	.align
	ldr r0, =GPBDAT			@ get value of GPBDAT var
	ldr r1, [r0]			@ get status of GPBDAT register
	orr r1, r1, #0x001		@ set r1[0]
	str r1, [r0]			@ GPB0 output 1.


.end
