#include "common.h"
#include "clk_pwr.h"
#include "memory.h"
#include "uart.h"
#include "print.h"

void board_init(void)
{
	printf("board_init() start\n");

	clk_pwr_init();
	memory_init();
	uart_init(UART0_PORT);

	printf("board_init() successfull.\n");
}
