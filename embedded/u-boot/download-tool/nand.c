#include "nand.h"

struct nandchip {
	u32 *ctrl;
	u32 *stat;
	u32 *cmmd;		//[7:0]
	u32 *addr;		//[7:0]
	u32 *data;
};

static struct nandchip nfchip;

static inline void select_chip(void)
{
	clear_bit(nfchip.ctrl, (0x1 << 1));
}

static inline void deselect_chip(void)
{
	set_bit(nfchip.ctrl, (1 << 1));
}

static inline void wait_ready(void)
{
	while (!(readl(nfchip.stat) & NF_RNB)) ;
}

static inline void nand_cmd(u32 cmd)
{
	select_chip();
	wait_ready();

	switch (cmd) {
	case NF_CMD_READ0:
	case NF_CMD_READ1:
	case NF_CMD_PGPROG0:
	case NF_CMD_PGPROG1:
	case NF_CMD_BLKES0:
	case NF_CMD_BLKES1:
	case NF_CMD_RDID:
	case NF_CMD_RDSTATUS:
		break;

	case NF_CMD_RESET:
		writel(nfchip.cmmd, NF_CMD_RESET & 0xff);
		wait_ready();
		break;

	default:
		break;
	}
	writel(nfchip.cmmd, cmd & 0xff);
}

enum {
	col_only,
	row_only,
	both_col_row,
};

static inline void send_address(u32 addr, u8 mode)
{
	u32 per_addr;

	if (addr > NF_MAX_ADDRESS)
		return;

	if ((mode == col_only) || (mode == both_col_row)) {
		/* 1st cycle */
		per_addr = addr & 0xff;
		writel(nfchip.addr, per_addr);

		/* 2nd cycle */
		per_addr = (addr >> 8) & 0x0f;
		writel(nfchip.addr, per_addr);
	}

	if ((mode == row_only) || (mode == both_col_row)) {
		/* 3rd cycle */
		per_addr = (addr >> 4) & 0xff;
		writel(nfchip.addr, per_addr);

		/* 4th cycle */
		per_addr = (addr >> 8) & 0xff;
		writel(nfchip.addr, per_addr);

		/* 5th cycle */
		per_addr = (addr >> 8) & 0x01;
		writel(nfchip.addr, per_addr);
	}
}

static inline struct nand_ctrl_t *get_nand_ctrl_base(void)
{
	return (struct nand_ctrl_t *)NAND_CTRL_ENTRY;
}

void nand_init()
{
	struct nand_ctrl_t *nand_ctrl = get_nand_ctrl_base();

	/* nand flash configuration register
	 * value = 0b 01 0010 0001 0000
	 * Note: Hclk time = 7.4ns
	 * [13:12] = 0b01, TACLS=1*7.4ns
	 * [10:8] = 0b010, TWRPH0=(0b10 + 1)*7.4ns
	 * [6:4] = 0b001, TWRPH1=(0b01 + 1)*7.4ns
	 * [3:0] hardware set
	 */
	writel(&nand_ctrl->nfconf, NFCONF);

	/* control register
	 * value = 0b
	 * [13] = 0b0, disable lock-tight
	 * [12] = 0b0, disable soft lock
	 * [10] = 0b0, disable interrupt
	 * [9] = 0b0, disable RnB interrupt
	 * [8] = 0b0, rising edge RnB detection
	 * [6] ,spare ECC lock
	 * [5] , main ECC lock
	 * [4] = 0b1, initECC
	 * [1] , chip select, 0:enable, 1:disable
	 * [0] 0b1, enable nand flash controller
	 */
	writel(&nand_ctrl->nfcont, NFCONT);

	/* cfg GPA17, 18, 19, 20, 22 as function pins */
	set_bit((u32 *) GPACON, (NF_CE | NF_RE | NF_WE | NF_ALE | NF_CLE));

	nfchip.ctrl = &nand_ctrl->nfcont;
	nfchip.stat = &nand_ctrl->nfstat;
	nfchip.cmmd = &nand_ctrl->nfcmmd;
	nfchip.addr = &nand_ctrl->nfaddr;
	nfchip.data = &nand_ctrl->nfdata;
}
