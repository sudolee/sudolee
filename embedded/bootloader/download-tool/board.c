#include "common.h"
#include "clk_pwr.h"
#include "memory.h"
#include "uart.h"
#include "print.h"

inline void board_init(void)
{
	clk_pwr_init();
	memory_init();
	uart_init(UART0_PORT);

	printf("Board init done\n");
}
