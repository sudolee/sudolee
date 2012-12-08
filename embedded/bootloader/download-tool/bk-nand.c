#include "nand.h"
#include "common.h"

#define MTD_MAX_ECCPOS_ENTRIES_LARGE 4

#if 1
#define NF_DEBUG(fmt, args...) printf(fmt, ##args)
#else
#define NF_DEBUG(fmt, args...) ({ 0;})
#endif

struct nand_info {
	u32 IO_NFCMD;
	u32 IO_NFADDR;
	u32 IO_NFDATA;

	u8 (*read_byte)(struct nand_info *this);
	u16 (*read_word)(struct nand_info *this);
	void (*read_buf)(struct nand_info *this, u8 *buf, int len);
	void (*write_buf)(struct nand_info *this, const u8 *buf, int len);

	void (*select_chip)(struct nand_info *this, int chip);
	void (*cmd_ctrl)(struct nand_info *this, int cmd, u32 ctrl);
	int (*waitfunc)(struct nand_info *this);
	int (*dev_ready)(struct nand_info *this);
	int (*erase)(struct nand_info *this, u32 addr, int len);
	int (*scan_bbt)(struct nand_info *this);

	struct nand_ecc_ctrl *ecc;

	/* nand flash controller register entry */
	struct nand_ctrl_t *ctrl_regs;

	u32 chip_delay; //5us after reset.

	/* size */
	int page_shift;
	int page_mask;
	int col_mask;
	int erase_shift;
	int writesize;
	//int oobsize = writesize >> 5;
};

/* ecc layout in oob area */
struct nand_ecclayout {
	u32 eccbytes;
	u32 eccpos[MTD_MAX_ECCPOS_ENTRIES_LARGE];
};

struct nand_ecc_ctrl {
	void (*hwctrl)(struct nand_info *this, int mode);
	int (*calculate)(struct nand_info *this, const u8 *data, u8 *ecc_code);
	int (*correct)(struct nand_info *this, u8 *data, u8 *new_ecc, u8 *orig_ecc);
	struct nand_ecclayout *layout;
};

struct nand_ops {
	u32 len;
	u32 retlen;
	u32 ooblen;
	u32 oobretlen;
	u8 *databuf;
	u8 *oobbuf;
};

/* local globl info */
static struct nand_info nf_info;

static struct nand_ecclayout ecc_layout = {
	.eccbytes = 3,
	.eccpos = {1, 2, 3}, /* byte 0 oob for bbt */
};

/* Export nf_info to all */
struct nand_info *get_nandinfo(void)
{
	return &nf_info;
}

static inline u8 nand_read_byte(struct nand_info *this)
{
	return readb(this->IO_NFDATA);
}

static inline u16 nand_read_word(struct nand_info *this)
{
	return readw(this->IO_NFDATA);
}

static void nand_read_buf(struct nand_info *this, u8 *buf, int len)
{
	readsl(this->IO_NFDATA, buf, len >> 2);

	if(len & 0x3)
		readsb(this->IO_NFDATA, buf + (len & ~0x3), len & 0x3);
}

static void nand_write_buf(struct nand_info *this, const u8 *buf, int len)
{
	writesl(this->IO_NFDATA, buf, len >> 2);

	if(len & 0x3)
		writesb(this->IO_NFDATA, buf + (len & ~0x3), len & 0x3);
}

static inline void s3c2440_nand_select_chip(struct nand_info *this, int chip)
{
	if(chip == -1)
		set_bit(&this->ctrl_regs->nfcont, S3C2440_NFSELBIT);
	else
		clear_bit(&this->ctrl_regs->nfcont, S3C2440_NFSELBIT);
}

static inline void nand_cmdctrl(struct nand_info *this, int cmd, u32 ctrl)
{
	if(ctrl & NAND_CTRL_CHANGE) {
		if(ctrl & NAND_nCE)
			s3c2440_nand_select_chip(this, NAND_nCE);
		else
			s3c2440_nand_select_chip(this, -1);
	}

	if(cmd != NAND_CMD_NONE) {
		if(ctrl & NAND_CLE)
			writeb(&this->ctrl_regs->nfcmmd, cmd);
		else
			writeb(&this->ctrl_regs->nfaddr, cmd);
	}
}

static int wait_func(struct nand_info *this)
{
	this->cmd_ctrl(this, NAND_CMD_STATUS, NAND_CTRL_CLE);

	return this->read_byte(this) & NAND_STATUS_READY;
}

static inline u8 check_chip_status(struct nand_info *this)
{
	this->cmd_ctrl(this, NAND_CMD_STATUS, NAND_CTRL_CLE);

	return this->read_byte(this);
}

static int nand_dev_ready(struct nand_info *this)
{
	return readb(&this->ctrl_regs->nfstat) & S3C2440_NFSTATUS_READY;
}

static void nand_command(struct nand_info *this, int cmd, int column, int page)
{
	int ctrl = NAND_CTRL_CLE;

#if 0
	if(cmd == NAND_CMD_SEQIN) {
		int readcmd;

		if (column > this->writesize) {
			/* OOB area */
			column -= this->writesize;
			readcmd = NAND_CMD_READOOB;
		}
	}
#endif

	this->cmd_ctrl(this, cmd, NAND_CTRL_CLE);

	/* send address ? */
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
		break;

	case NAND_CMD_RESET:
		udelay(this->chip_delay); /* 5us */
		while(wait_func(this))
			;
		break;

	default:
		ndelay(40); /* about 100ns */
		while(this->dev_ready(this))
			;
	}
}

static int check_offs_len(struct nand_info *this, u32 addr, int len)
{
	int ret = 0;

	if(addr & ((1 << this->erase_shift) - 1)) {
		NF_DEBUG("%s() unaligned address.\n", __func__);
		ret = -EINVAL;
	}

	if(len & ((1 << this->erase_shift) - 1)) {
		NF_DEBUG("%s() length not block aligned.\n", __func__);
		ret = -EINVAL;
	}

	return ret;
}

static int nand_check_wp(struct nand_info *this)
{
	this->cmd_ctrl(this, NAND_CMD_STATUS, NAND_CTRL_CLE);

	return this->read_byte(this) & NAND_STATUS_WP;
}

static inline void erase_cmd(struct nand_info *this, int page)
{
	nand_command(this, NAND_CMD_ERASE1, -1, page);
	nand_command(this, NAND_CMD_ERASE2, -1, -1);
}

static int nand_erase(struct nand_info *this, u32 addr, int len)
{
	int ret;
	int page, pages_per_block;

	if(check_offs_len(this, addr, len)) {
		NF_DEBUG("nand_erase() invalid block %d\n", 1);
		return -EINVAL;
	}

	/* chip select */
	this->cmd_ctrl(this, NAND_CMD_NONE, NAND_CTRL_CHANGE | NAND_nCE);

	/* is it write protected ? */
	if(nand_check_wp(this)) {
		NF_DEBUG("nand_erase: device is write protected!!!\n");
		goto erase_exit;
	}

	page = (int)(addr >> this->page_shift);
	pages_per_block = 1 << (this->erase_shift - this->page_shift);

	do {
		erase_cmd(this, page);

		while(wait_func(this))
			;

		if(check_chip_status(this) & NAND_STATUS_FAIL) {
			NF_DEBUG("%s: Failed erase, page 0x%x\n", __func__, page);
			goto erase_exit;
		}

		/* increase address and decrease length */
		page += pages_per_block;
		len -= 1 << this->erase_shift;

		/* cross chip ? */
		if(len && !(page & this->page_mask)) {
			NF_DEBUG("%s() cross a chip boundary\n", __func__);
			goto erase_exit;
		}
	} while((len > 0) && (page & this->page_mask));
	ret = NAND_ERASE_DONE;

erase_exit:
	/* chip deselect */
	this->cmd_ctrl(this, NAND_CMD_NONE, NAND_CTRL_CHANGE);

	return (ret == NAND_ERASE_DONE) ? NAND_ERASE_DONE : -EIO;
}

#if 0
static int nand_do_read_ops(struct nand_info *this, u32 from, struct nand_ops *ops)
{
	int page, realpage, col, bytes, aligned;
	int ret = 0;
	u32 readlen = ops->len;
	u32 oobreadlen = ops->ooblen;

	this->select_chip(this, 1);

	realpage = (int)(from >> this->page_shift);
	page = realpage & this->page_mask;

	col = from & (this->writesize - 1);

	while(1) {
		bytes = min(readlen, this->writesize - col);
		aligned = (bytes == this->writesize);

		this->cmd_ctrl(this, NAND_CMD_READ0, NAND_CTRL_CLE);

		/*
		if(aligned)
			this->read_page();
		else
			this->read_subpage();
		*/
	}

	return 0;
}
#endif

static int nand_do_write_ops(struct nand_info *this, u32 to, struct nand_ops *ops)
{
	return ops->len;
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
	writel(&this->ctrl_regs->nfconf, (0x01 << 12) | (0x02 << 8) | (0x01 << 4));

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
	writel(&this->ctrl_regs->nfcont, 0x1 << 0);

	/* cfg GPA 17, 18, 19, 20, 22 as function pins */
//	set_bit((u32 *) GPACON, (NF_nCE | NF_RE | NF_WE | NF_ALE | NF_CLE));
	set_bit((u32 *) GPACON, (0x1 << 17) | (0x1 << 18) | (0x1 << 19) | (0x1 << 20) | (0x1 << 22));

	/* reset nand chip in the end */
	nand_command(this, NAND_CMD_RESET, -1, -1);
}

void nand_module_init(void)
{
	struct nand_ctrl_t *regs = (struct nand_ctrl_t *)NAND_CTRL_ENTRY;

	NF_DEBUG("nand_module_init() start...\n");

	nf_info.IO_NFCMD = (u32)&regs->nfcmmd;
	nf_info.IO_NFADDR = (u32)&regs->nfaddr;
	nf_info.IO_NFADDR = (u32)&regs->nfdata;
	nf_info.ctrl_regs = regs;

	nf_info.chip_delay = 5; // 5us

	/*
	 * column address: A0 ~ A11
	 * page address: A12 ~ A28
	 * block address: A18 ~ A28
	 * page size: 2048 (+ 64 oob)
	 * block size: 64 * pages
	 */
	nf_info.page_shift = 12;
	nf_info.page_mask = 0x1FF; /* bit0 ~ bit16, i.e. A12 ~ A28 */
	nf_info.erase_shift = 18;
	nf_info.writesize = 2048;
	//nf_info.oobsize = 2048 >> 5;

	/* functions init... */
	nf_info.select_chip = s3c2440_nand_select_chip;
	nf_info.read_byte = nand_read_byte;
	nf_info.read_word = nand_read_word;
	nf_info.cmd_ctrl = nand_cmdctrl;

	nf_info.read_buf = nand_read_buf;
	nf_info.write_buf = nand_write_buf;
	nf_info.waitfunc = wait_func;
	nf_info.dev_ready = nand_dev_ready;
	nf_info.erase = nand_erase;
	nf_info.scan_bbt = NULL;

	nf_info.ecc->hwctrl = NULL;
	nf_info.ecc->calculate = NULL;
	nf_info.ecc->correct = NULL;
	nf_info.ecc->layout = &ecc_layout;



	nandhw_init(&nf_info);
	NF_DEBUG("The end of nand init.\n");
}
