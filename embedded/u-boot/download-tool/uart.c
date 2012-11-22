#include "uart.h"


static inline struct uart_t *get_uart_base(u32 index)
{
	switch(index){
		case 0:
			return (struct uart_t *)UART0_ENTRY;
		case 1:
			return (struct uart_t *)UART1_ENTRY;
		case 2:
			return (struct uart_t *)UART2_ENTRY;
		default:
			return NULL;
	};
}

void uart_init(u32 index)
{
	struct uart_t *uart = get_uart_base(index);

	if(uart) {
		writel(&uart->ulcon, 0);
	}
}
