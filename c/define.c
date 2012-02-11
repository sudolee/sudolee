#include <stdio.h>
#define DEBUG 1

void callee(char *str)
{
	printf
	    (">I'm %s which is in %s...\n>This is the %dth line...\n>I get some mesg: %s\n",
	     __func__, __FILE__, __LINE__, str);
}

int main(int argc, char *argv[])
{
	char *string = "Hello world...";

	callee(string);

	return 0;
}
