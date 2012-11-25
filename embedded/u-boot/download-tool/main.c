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

	for(i = 0;; i++) {
//		puts(UART0_PORT, "Hello, the cruel world...\n");
		led_run(i);
		delay(0x006fffff);
		i %= 4;
		printf("%4x, %o -- %p, %s\n", 19880402, 19880402, &i, "Hello, the cruel world...\n");
	}

	asm("b .");	/* hang there */
}
