#ifndef _COMMON_H_
#define _COMMON_H_

#include <iostream>

class Hello
{
public:
	void print(const char *str);

	Hello(void){
		std::cout << "Beginning..." << std::endl;
	}
	~Hello(void){
		std::cout << "Ending..." << std::endl;
	}
};

void Hello::print(const char *str)
{
	std::cout << ">>> " 
			  << str
			  << std::endl;
}

#endif
