#include "clk_pwr.h"
#include "memory.h"
#include "uart.h"

#include "debug.h"

void c_start(void)
{
	/* init table */
	clk_pwr_init();
	led_run(0);

	memory_init();
	led_run(1);

	/* Only uart0 in use */
	uart_init(UART0_PORT);

	asm("b .");	/* hang there */
}
