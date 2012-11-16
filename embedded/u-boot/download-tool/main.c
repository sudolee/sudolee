#include "clock.h"

void memory_init(void)
{

}

void c_start(void)
{
	clock_power_init();
	mb();
	nop;
}
