/*
 * Little program for downloading binary into nand/nor flash (QT2440).
 * No parallel and serial port. With JLink only.
 */

.section .asm.text
.globl _start

.extern c_start

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
	bic r1, r1, #0x001		@ clear r1[0]
	str r1, [r0]			@ GPB0 output 0, disable beep.

mmu_stuff:
	mrc p15, 0, r0, c1, c0, 0
	bic r0, r0, #0x00002300
	bic r0, r0, #0x00000085      @ disable: bit 0, mmu; bit 2, d-cache
	orr r0, r0, #0x00001000      @ enable: bit 12, i-cache
	orr r0, r0, #0x00000002      @ enable: bit 1, align
	mcr p15, 0, r0, c1, c0, 0

	b c_start

.end
