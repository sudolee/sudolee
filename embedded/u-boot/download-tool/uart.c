#include "uart.h"

#define RX_FIFO_EMPTY (0x3f)
#define TX_FIFO_FULL (1 << 14)

struct uartlite {
	u8 *rx; /* [7:0] little-endian, [31:24] big-endian */
	u8 *tx; /* Same to rx buffer */
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
	}
}

/* Send functions */
void putc(const u32 port, const char ch)
{
	if(ch == '\n')
		putc(port, '\r');

	/* is tx fifo full ? */
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
	/* is any data in rx fifo ? */
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
	uart_ports[port].rx = &entry->rxdata;
	uart_ports[port].tx = &entry->txdata;
	uart_ports[port].status = &entry->ufstat;
}

void uart_init(u32 index)
{
	struct uart_t *uart = get_uart_base(index);

	if(uart) {
		/* config uart function pins */
		switch(index) {
#define GPH_UART0 (0xaa << 0)
#define GPH_UART1 (0xa << 8)
#define GPH_UART2 (0xa << 12)
			case UART0_PORT:
				set_bit((u32 *)GPHCON, GPH_UART0);
				break;

			case UART1_PORT:
				set_bit((u32 *)GPHCON, GPH_UART1);
				break;

			case UART2_PORT:
				set_bit((u32 *)GPHCON, GPH_UART2);
				break;
			default:
				/* Nothing to do */
				break;
		}

		/* uart line control register
		 * value	= 0b 010 0011
		 * [6]		= 0b0	: normal mode
		 * [5:3]	= 0b100	: odd parity
		 * [2]		= 0b0	: 1 stop bit
		 * [1:0]	= 0b11	: 8-bits data
		 */
		writel(&uart->ulcon, (0x3 << 0) | (0x4 << 3));
		
		/* uart control register
		 * value	= 0b 0000 0000 0101
		 * [15:12]	= not used	: Fclk divider
		 * [11:10]	= 0b00/0b10	: clk select Pclk
		 * [9]		= not used	: Tx int type
		 * [8]		= not used	: Rx int type
		 * [7]		= not used	: Rx timeout int disable
		 * [6]		= note used	: Rx err int disable
		 * [5]		= 0b0		: normal operation
		 * [4]		= 0b0		: normal transmit
		 * [3:2]	= 0b01		: int or polling
		 * [1:0]	= 0b01		: int or polling
		 */
		writel(&uart->ucon, (0x1 << 2) | (0x1 << 0));

		/* uart FIFO control register
		 * value = 0x1
		 * [7:6]	= 0b	: Tx trigger lvl
		 * [5:4]	= 0b	: Rx trigger lvl
		 * [2]		= 0b0	: Tx FIFO reset normal
		 * [1]		= 0b0	: Rx FIFO reset normal
		 * [0]		= 0b1	: enable FIFO
		 */
		writel(&uart->ufcon, (0x1 << 0));

		/* ubrdiv = (int)(uart clk / (buad rate * 16)) - 1
		 * value = (int)((405Mhz/6)/(115200 * 16)) - 1 = 0x24
		 */
		writel(&uart->ubrdiv, 0x24);

		port_init(index, uart);
	}
}
