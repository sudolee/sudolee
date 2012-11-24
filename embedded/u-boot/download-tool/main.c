#include "clk_pwr.h"
#include "memory.h"
#include "uart.h"

#include "debug.h"

void board_init(void)
{
	u8 i;
	/* init table */
	clk_pwr_init();
	led_run(0);

	memory_init();
	led_run(1);

	/* Only uart0 in use */
	uart_init(UART0_PORT);
	led_run(2);

	for(i = 0;; i++) {
		puts(UART0_PORT, "hello world...\n");
		led_run(i);
		delay(0x006fffff);
		i %= 4;
	}

	asm("b .");	/* hang there */
}
