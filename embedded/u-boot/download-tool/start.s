/*
 * Little program for downloading binary into nand/nor flash (QT2440).
 * No parallel and serial port. With JLink only.
 */

.section .asm.text
.arm
.globl _start

.extern c_start
.extern led_init


_start:
	b start_code
	.rept 7					@ For 7 exceptions
		b .
	.endr

start_code:
#	mrs r0, cpsr
#	bic r0, r0, #0x1f
#	orr r0, r0, #0xd3
#	msr cpsr_c, r0

disable_watchdog:
	.align
	.equ WTCON, 0x53000000	@ watchdog timer control register
	ldr r0, =WTCON			@ get value of WTCON var
	mov r1, #0x0
	str r1, [r0]			@ clear WTCON register

	bl led_init				@ init led for debug and disable beep

mmu_stuff:
	mrc p15, 0, r0, c1, c0, 0
	bic r0, r0, #0x00002300
#	bic r0, r0, #0x00000085      @ disable: bit 0, mmu; bit 2, d-cache
	bic r0, r0, #0x00000087      @ disable: bit 0, mmu; bit 2, d-cache
	orr r0, r0, #0x00001000      @ enable: bit 12, i-cache
	orr r0, r0, #0x00000002      @ enable: bit 1, align
	mcr p15, 0, r0, c1, c0, 0

	.align
	/* sp must align to 4Byte */
	.equ SP_ENTRY, 0x40000ffc	@ Nor flash boot
#	.equ SP_ENTRY, 0x00000fff	@ nand flash boot
	ldr sp, =SP_ENTRY			@ this tool only run in steppingstone.

	b c_start

.end
