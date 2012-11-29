#include "common.h"
#include "clk_pwr.h"
#include "memory.h"
#include "uart.h"
#include "print.h"

#include "debug.h"

void board_init(void)
{
	u8 i;
	/* init table */
	clk_pwr_init();
	memory_init();
	uart_init(UART0_PORT);

	for (i = 0;; i++) {
		led_run(i);
		ndelay(0x006fffff);
		i %= 4;
		serial_printf(UART0_PORT, "%d, %s\n", i, "Hello, the cruel world...\n");
	}

	asm("b .");		/* hang there */
}
