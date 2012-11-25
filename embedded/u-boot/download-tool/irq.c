#include "irq.h"

static inline struct irq_regs_t *get_irq_regs_base(void)
{
	return (struct irq_regs_t *)IRQ_ENTRY;
}

inline void msk_irq(u32 index)
{
	struct irq_regs_t *base = get_irq_regs_base();

	if (!(index > MAX_SUBIRQ_NUM))
		writel(&base->intmsk, 1 << index);
}

inline void msk_subirq(u32 index)
{
	struct irq_regs_t *base = get_irq_regs_base();

	if (!(index > MAX_SUBIRQ_NUM))
		writel(&base->intsubmsk, 1 << index);
}

u32 clear_irq_pnd(u32 index)
{
	//struct irq_regs_t *base = get_irq_regs_base();

	 /*TODO*/
	    /*
	       which irq ?
	       if (is subirq)
	       clear subsrcpnd;
	       clear srcpnd;
	       clear intpnd;
	     */
	    return 0;
}

void irq_init(void)
{
	struct irq_regs_t *base = get_irq_regs_base();

	/* clear all bits via write "1" */
	writel(&base->srcpnd, 0xffffffff);

	/* set all irq as IRQ (no fiq) */
	writel(&base->intmod, 0xffffffff);

	/* only enable UART0 so far */
	writel(&base->intmsk, ~(1 << INT_UART0));

	/* default with rorate priority */
	writel(&base->priority, 0x7f);

	/* clear all bits via write "1" */
	writel(&base->intpnd, 0xffffffff);

	/* clear all bits via write "1" */
	writel(&base->subsrcpnd, 0x7f);

	/* only enable RXD0, TXD0, ERR0 of UART0 so far */
	writel(&base->intsubmsk, (1 << INT_RXD0) | (1 << INT_TXD0) | (1 << INT_ERR0));

	__enable_irq();
	__disable_irq();
}

void irq_route(void)
{
}
