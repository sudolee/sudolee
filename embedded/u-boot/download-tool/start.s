/*
 * Little program for downloading binary into nand/nor flash (QT2440).
 * No parallel and serial port. With JLink only.
 */

.section .asm.text
.arm
.globl _start

.extern board_init
.extern dt_main


_start:
	b start_code
	b .
	b .
	b .
	b .
	nop	
	ldr pc, __irq
	b .

/*
	ldr pc, __undefined_instruction
	ldr pc, __swi_interrupt
	ldr	pc, __prefetch_abort
	ldr pc, __data_abort
	nop
	ldr pc, __irq
	ldr pc, __fiq
*/
__irq:
	.long 0xdeadbeef @ TODO: irq_route

start_code:
	mrs r0, cpsr
	bic r0, r0, #0x1f
	orr r0, r0, #0xd3
	msr cpsr_c, r0

disable_watchdog:
	.equ WTCON, 0x53000000	@ watchdog timer control register
	ldr r0, =WTCON			@ get value of WTCON var
	mov r1, #0x0
	str r1, [r0]			@ clear WTCON register

#	bl led_init				@ init led for debug and disable beep

mmu_stuff:
	mrc p15, 0, r0, c1, c0, 0
	bic r0, r0, #0x00002300
#	bic r0, r0, #0x00000085      @ disable: bit 0, mmu; bit 2, d-cache
	bic r0, r0, #0x00000087      @ disable: bit 0, mmu; bit 2, d-cache
	orr r0, r0, #0x00001000      @ enable: bit 12, i-cache
	orr r0, r0, #0x00000002      @ enable: bit 1, align
	mcr p15, 0, r0, c1, c0, 0

/* set stack pointer */
	/* Note: sp must align to 4 byte */
#	.equ SP_ENTRY, 0x40000ffc	@ Nor flash boot
	.equ SP_ENTRY, 0x00000ffc	@ nand flash boot
	ldr sp, =SP_ENTRY			@ this tool only run in steppingstone.

	bl board_init

	.equ SDRAM_SP_POINTER, 0x33fffffc	@ sp pointer at the top of sdram 0~(64MB - 1 & 0x3)
	ldr sp, =SDRAM_SP_POINTER

	bl dt_main

	ldr pc, =start_on_sdram
start_on_sdram:
	b .

.end
