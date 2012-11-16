.section .data
output:
	.ascii "hello world...\n"
output_end:
.equ len, output_end - output

.equ SYSCALL_WRITE, 4
.equ SYSCALL_EXIT, 1
.equ STD_OUT, 1
.equ EXIT_STATE, 0

.section .text
.globl main
main:
	movl $SYSCALL_WRITE, %eax
	movl $STD_OUT, %ebx
	movl $output, %ecx
	movl $len, %edx
	int $0x80
	movl $SYSCALL_EXIT, %eax
	movl $EXIT_STATE, %ebx
	int $0x80
