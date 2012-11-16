void func(void)
{
	char *string = "hello world...\n";
	int len = 15;//strlen(string);

	__asm__ __volatile__("movl $4, %%eax \n\t"
			"movl $1, %%ebx \n\t"
			"movl %0, %%ecx \n\t"
			"movl %1, %%edx \n\t"
		    "int $0x80"
			::"m"(string), "m"(len):);
}

void exit()
{
	__asm__ __volatile__("movl $1, %eax \n\t"
			"movl $0, %ebx \n\t"
			"int $0x80");
}

int mymain()
{
	func();
	exit();
}
