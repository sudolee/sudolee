/*
 * Little program on QT2440.
 * Shift to light off each led, while beep beeps at last time.
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

prepares:
	.equ GPBCON, 0x56000010		@ Port B control register
	.equ GPBDAT, 0x56000014		@ Port B data register
	.align
	ldr r0, =GPBCON			@ get value of GPBCON var
	ldr r1, [r0]			@ get status of GPBCON register
#	bic r1, r1, #0x0003fc03 @ bad immediate
	bic r1, r1, #0x0003fc00 @ clear r1[17:10]
	bic r1, r1, #0x00000003 @ clear r1[1:0]
#	orr	r1, r1, #0x00015401
	orr	r1, r1, #0x00015400
	orr	r1, r1, #0x00000001
	str r1, [r0]			@ set GPB0,5,6,7,8 as output port

	.align
	ldr r0, =GPBDAT			@ get value of GPBDAT var
	ldr r1, [r0]			@ get status of GPBDAT register
#	bic r1, r1, #0x1e1		@ clear r1[8:5, 0]
	bic r1, r1, #0x1e0
	bic r1, r1, #0x001

/*
 * GPB0 connected to beep, 1 enable, 0 disable.
 * GPB(5~8) connected to nLED_(1~4), 1 enable, 0 disable.
 */
Leds_run:
	mov r3, #5				@ shift left at least 5 bits
	mov r4, #1				@ enable port anfter shift
0:
	orr r2, r1, r4, lsl r3	@ set r2 as new status we want to output to GPBDAT
	cmp r3, #8				@ shift end ?
	addeq r2, r2, #1		@ if the last led, additional beep beeps
	str r2, [r0]
	bl delay				@ sleep for a memont, cpsr_f changed.
	cmp r3, #8				@ shift end ?
	addne r3, r3, #1		@ next one ready.
	bne 0b
	b Leds_run				@ shift end, once again.
	
delay:
	mov r5, #204800
1:
	subs r5, r5, #1			@ Note: cpsr_f changed, bits[31:24]
	bne 1b
	mov pc, lr				@ return to Leds_run

.end
