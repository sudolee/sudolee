#include "uart.h"

struct uartlite {
	u8 *rx;		/* Rx buffer. [7:0] little_endian, [31:24] big_endian */
	u8 *tx;		/* Tx buffer. The same to rx */
	u32 *status; /* fifo status */
};

static struct uartlite uart_ports[3];

static inline struct uart_t *get_uart_base(u32 index)
{
	switch(index){
		case UART0_PORT:
			return (struct uart_t *)UART0_ENTRY;
		case UART1_PORT:
			return (struct uart_t *)UART1_ENTRY;
		case UART2_PORT:
			return (struct uart_t *)UART2_ENTRY;
		default:
			return NULL;
	};
}

/* Send functions */
void putc(const u32 port, const char ch)
{
	if(ch == '\n')
		putc(port, '\r');

	/* tx fifo is full ? */
	while(readl(uart_ports[port].status) & TX_FIFO_FULL)
		;
	writeb(uart_ports[port].tx, ch);
}

void puts(const u32 port, const char *s)
{
	while(*s)
		putc(port, *s++);
}

/* Receive functions */
char getc(const u32 port)
{
	/* any data in rx fifo ? */
	while(!(readl(uart_ports[port].status) & RX_FIFO_EMPTY))
		;
	return readb(uart_ports[port].rx);
}

__attribute__((weak)) char * gets(const u32 port)
{
	return NULL;
}

/* init uartlite structure */
static void port_init(u32 port, struct uart_t *entry)
{
	uart_ports[port].rx = &entry->urxh;
	uart_ports[port].tx = &entry->utxh;
	uart_ports[port].status = &entry->ufstat;
}

void uart_init(u32 index)
{
	struct uart_t *uart = get_uart_base(index);

	if(uart) {
		/* uart line control register
		 * value = 0b 010 0011
		 * [6] 0b0: normal mode
		 * [5:3] 0b100: odd parity
		 * [2] 0b0: 1 stop bit
		 * [1:0] 0b11: 8-bits data
		 */
		writel(&uart->ulcon, ULCON);
		
		/* uart control register
		 * value = 0b 0000 0000 0101
		 * [15:12] Fclk divider
		 * [11:10] 0b00/0b10: clk select Pclk
		 * [9] Tx int type
		 * [8] Rx int type
		 * [7] 0b0: Rx timeout int disable
		 * [6] 0b0: Rx err int disable
		 * [5] 0b0: normal operation
		 * [4] 0b0: normal transmit
		 * [3:2] 0b01: int or polling
		 * [1:0] 0b01: int or polling
		 */
		writel(&uart->ucon, UCON);

		/* uart FIFO control register
		 * value = 0x1
		 * [7:6] 0b: Tx trigger lvl
		 * [5:4] 0b: Rx trigger lvl
		 * [3]
		 * [2] 0b0: Tx FIFO reset normal
		 * [1] 0b0: Rx FIFO reset normal
		 * [0] 0b1: enable FIFO
		 */
		writel(&uart->ufcon, UFCON);

		/* ubrdiv = (int)(uart clk / (buad rate * 16)) - 1
		 * value = (int)((504Mhz/6)/(115200 * 16)) - 1 = 45.57-1 = 46-1 = 45
		 */
		writel(&uart->ubrdiv, UBRDIV);

		port_init(index, uart);
	}
}
