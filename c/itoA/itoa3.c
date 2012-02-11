#include <stdio.h>

char *itoa(int value, char *str, int radix) 
{ 
	int num, i;
	char *ptr, temp;
	unsigned int n;
	
	static char table[] =
	{ 
		'0', '1', '2', '3', '4', '5', 
		'6', '7', '8', '9', 'a', 'b', 
		'c', 'd', 'e', 'f', 'g', 'h', 
		'i', 'j', 'k', 'l', 'm', 'n', 
		'o', 'p', 'q', 'r', 's', 't', 
		'u', 'v', 'w', 'x', 'y', 'z' 
	};
	
	num = -1; 
	ptr = str; 
	if(radix >= 2 && radix <= 36) 
	{   
		if(value < 0 && radix == 10) 
		{ 
			*ptr++ = '-'; 
			value = -value; 
		} 
		
		n = *(unsigned *)&value; 
		
		do{ 
		ptr[++num] = table[n % radix]; 
		n /= radix; 
		}while(n > 0); 
		
#if 1
		i = (num + 1) / 2;  
		
		while(i-- > 0)
		{  
			temp = ptr[i]; 
			ptr[i] = ptr[num - i]; 
			ptr[num - i] = temp; 
		} 
#endif
		
	} 
	ptr[num + 1] = '\0';
	return str; 
} 

int main(void)
{
	char ch[13];

	printf("%s", itoa(256, ch, 10));

	return 0;
}
