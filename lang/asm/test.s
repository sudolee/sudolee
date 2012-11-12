.section .data
output:
	.ascii "hello world...\n"
.equ SYSCALL_WRITE_NO, 4
.equ SYSCALL_EXIT_NO, 1
.equ LEN_OF_STRING, 15
.equ STD_OUT, 1
.equ EXIT_STATE, 0
buffer:
	.fill 1024, 2, 0

.section .text
.globl main
main:
	movl $SYSCALL_WRITE_NO, %eax
	movl $STD_OUT, %ebx
	movl %esp, %ecx
	movl $2, %edx
	int $0x80
	movl $SYSCALL_EXIT_NO, %eax
	movl $EXIT_STATE, %ebx
	int $0x80
