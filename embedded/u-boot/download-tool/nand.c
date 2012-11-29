#include "nand.h"
#include "common.h"

#define MTD_MAX_ECCPOS_ENTRIES_LARGE 16
#define MTD_MAX_OOBFREE_ENTRIES_LARGE 4

struct nand_info {
	void *IO_NFCMD;
	void *IO_NFADDR;
	void *IO_NFDATA;

	u8 (*read_byte)(struct nand_info *this);
	u16 (*read_word)(struct nand_info *this);
	void (*read_buf)(struct nand_info *this, u8 *buf, int len);
	void (*write_buf)(struct nand_info *this, const u8 *buf, int len);

	void (*select_chip)(struct nand_info *this, int chip);
	void (*cmd_ctrl)(struct nand_info *this, int cmd, u32 ctrl);
	int (*waitfunc)(struct nand_info *this);
	int (*dev_ready)(struct nand_info *this);
	void (*erase_cmd)(struct nand_info *this, int page);
	int (*scan_bbt)(struct nand_info *this);

	struct nand_ecc_ctrl *ecc;

	/* nand flash controller register entry */
	struct nand_ctrl_t *ctrl_regs;

	u32 chip_delay; //5us after reset.
};

struct nand_oobfree {
	u32 offset;
	u32 length;
};

struct nand_ecclayout {
	u32 eccbytes;
	u32 eccpos[MTD_MAX_ECCPOS_ENTRIES_LARGE];
	struct nand_oobfree oobfree[MTD_MAX_OOBFREE_ENTRIES_LARGE];
};

struct nand_ecc_ctrl {
	void (*hwctrl)(struct nand_info *this, int mode);
	int (*calculate)(struct nand_info *this, const u8 *data, u8 *ecc_code);
	int (*correct)(struct nand_info *this, u8 *data, u8 *new_ecc, u8 *orig_ecc);
	struct nand_ecclayout *layout;
};

/* local globl info */
static struct nand_info nf_info;

static inline u8 nand_read_byte(struct nand_info *this)
{
	return readb(this->IO_NFDATA);
}

static inline u16 nand_read_word(struct nand_info *this)
{
	return readw(this->IO_NFDATA);
}

static inline void nand_cmdctrl(struct nand_info *this, int cmd, u32 ctrl)
{
	if(cmd == NAND_CMD_NONE)
		return;

	if(ctrl & NAND_CLE)
		writeb(this->ctrl_regs + S3C2440_NFCMD, cmd);
	else
		writeb(this->ctrl_regs + S3C2440_NFADDR, cmd);
}

static inline void s3c2440_nand_select_chip(struct nand_info *this, int chip)
{
	/* TODO: enable nand clk *
	if(chip != -1) {
	} */

	if(chip == -1) {
		set_bit(this->ctrl_regs + S3C2440_NFCONT, S3C2440_NFSELBIT);
	} else {
		clear_bit(this->ctrl_regs + S3C2440_NFCONT, S3C2440_NFSELBIT);
	}

	/* TODO: disable nand clk *
	if(chip == -1) {
	} */
}

void nandhw_init(struct nand_info *this)
{

	/* nand flash configuration register
	 * value = 0b 01 0010 0001 0000
	 * Note: Hclk time = 7.4ns
	 * [13:12]	= 0b01		: TACLS=1*7.4ns
	 * [10:8]	= 0b010		: TWRPH0=(0b10 + 1)*7.4ns
	 * [6:4]	= 0b001		: TWRPH1=(0b01 + 1)*7.4ns
	 * [3:0]				: hardware set
	 */
	writel(nf_info.ctrl_regs->nfconf, 0x1210);

	/* control register
	 * value = 0b
	 * [13]	= 0b0	: disable lock-tight
	 * [12] = 0b0	: disable soft lock
	 * [10] = 0b0	: disable interrupt
	 * [9]	= 0b0	: disable RnB interrupt
	 * [8]	= 0b0	: rising edge RnB detection
	 * [6] 			: spare ECC lock
	 * [5] 			: main ECC lock
	 * [4]	= 0b1	: initECC
	 * [1] 			: chip select, 0:enable, 1:disable
	 * [0]	= 0b1	: enable nand flash controller
	 */
	writel(nf_info.ctrl_regs->nfcont, 0x1);

	/* cfg GPA 17, 18, 19, 20, 22 as function pins */
//	set_bit((u32 *) GPACON, (NF_CE | NF_RE | NF_WE | NF_ALE | NF_CLE));
	set_bit((u32 *) GPACON, (0x1 << 17) | (0x1 << 18) | (0x1 << 19) | (0x1 << 20) | (0x1 << 22));

#if 0
	nfchip.ctrl = &nand_ctrl->nfcont;
	nfchip.stat = &nand_ctrl->nfstat;
	nfchip.cmmd = &nand_ctrl->nfcmmd;
	nfchip.addr = &nand_ctrl->nfaddr;
	nfchip.data = &nand_ctrl->nfdata;
#endif

	nand_common(this, NAND_CMD_RESET, -1, -1);
	/* TODO: reset nand in the end */
}

static void nand_command(struct nand_info *this, u32 cmd, int column, int page)
{
	int ctrl = NAND_CTRL_CLE;

#if 0
	if(cmd == NAND_CMD_SEQIN) {
		int readcmd;

		if (column > NAND_WRITESIZE) {
			/* OOB area */
			column -= NAND_WRITESIZE;
			readcmd = NAND_CMD_READOOB;
		}
	}
#endif

	ctrl = NAND_CTRL_CLE;
	this->cmd_ctrl(this, cmd, ctrl);

	ctrl = NAND_CTRL_ALE;
	if(column != -1) {
		/* column addr: A0~A7 */
		this->cmd_ctrl(this, (column >> 0) & 0xff, ctrl);
		/* column addr: A8~A11 */
		this->cmd_ctrl(this, (column >> 8) & 0x0f, ctrl);
	}

	if(page != -1) {
		/* row addr: A12~A19 */
		this->cmd_ctrl(this, (page >>  0) & 0xff, ctrl);
		/* row addr: A20~A27 */
		this->cmd_ctrl(this, (page >>  8) & 0xff, ctrl);
		/* row addr: A28 */
		this->cmd_ctrl(this, (page >> 16) & 0x01, ctrl);
	}

	switch(cmd) {
	case NAND_CMD_PAGEPROG:
	case NAND_CMD_ERASE1:
	case NAND_CMD_ERASE2:
	case NAND_CMD_SEQIN:
	case NAND_CMD_STATUS:
		return;

	case NAND_CMD_RESET:
		udelay(5);
		this->cmd_ctrl(this, NAND_CMD_STATUS, NAND_CTRL_CLE);
		while(!(this->read_byte(this) & NAND_STATUS_READY))
			;
		return;

	default:
		ndelay(40); //about 100ns
		this->dev_ready(this);
	}
}

void nand_module_init(void)
{
	struct nand_ctrl_t *regs = (struct nand_ctrl_t *)NAND_CTRL_ENTRY;

	nf_info.IO_NFCMD = &regs->nfcmmd;
	nf_info.IO_NFADDR = &regs->nfaddr;
	nf_info.IO_NFADDR = &regs->nfdata;
	nf_info.ctrl_regs = regs;

//	nf_info.chip_delay = 50;

	/* functions init... */
	nf_info.select_chip = s3c2440_nand_select_chip;
	nf_info.read_byte = nand_read_byte;
	nf_info.read_word = nand_read_word;
	nf_info.cmd_ctrl = nand_cmdctrl;

	/*
	nf_info.read_buf
	nf_info.write_buf
	nf_info.waitfunc
	nf_info.dev_ready
	nf_info.erase_cmd
	nf_info.scan_bbt
	*/




}


