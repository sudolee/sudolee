#include "common.h"
#include "string.h"

#include "nand.h"

#if 1
#define NF_DEBUG(fmt, args...) printf(fmt, ##args)
#else
#define NF_DEBUG(fmt, args...) ({0;})
#endif

/* local globl info */
static struct nand_info nf_info;

/* Export nf_info to all */
inline struct nand_info *get_nandinfo(void)
{
	return &nf_info;
}

static inline void nand_read_buf(struct nand_info *this, char *buf, int len)
{
	readsl(this->IO_NFDATA, buf, len >> 2);

	if(len & 0x3)
		readsb(this->IO_NFDATA, buf + (len & ~0x3), len & 0x3);
}

static inline void nand_write_buf(struct nand_info *this, const char *buf, int len)
{
	writesl(this->IO_NFDATA, buf, len >> 2);

	if(len & 0x3)
		writesb(this->IO_NFDATA, buf + (len & ~0x3), len & 0x3);
}

static inline void nand_cmdctrl(struct nand_info *this, int cmd, u32 ctrl)
{
	if(ctrl & NAND_CTRL_CHANGE) {
		if(ctrl & NAND_nCE)
			clear_bit(&this->ctrl_regs->nfcont, S3C2440_NFSELBIT);
		else
			set_bit(&this->ctrl_regs->nfcont, S3C2440_NFSELBIT);
	}

	if(cmd != NAND_CMD_NONE) {
		if(ctrl & NAND_CLE)
			writeb(this->IO_NFCMD, cmd);
		else
			writeb(this->IO_NFADDR, cmd);
	}
}

#if 0
#define check_chip_status(this) ({ \
	nand_cmdctrl(this, NAND_CMD_STATUS, NAND_CTRL_CLE); \
	readb(this->IO_NFDATA);})
#endif

static inline int check_chip_status(struct nand_info *this)
{
	nand_cmdctrl(this, NAND_CMD_STATUS, NAND_CTRL_CLE);
	return readb(this->IO_NFDATA);
}

static inline void nand_command(struct nand_info *this, int cmd, int column, int page)
{
	int ctrl;

	nand_cmdctrl(this, cmd, NAND_CTRL_CLE);

	/* send address ? */
	ctrl = NAND_CTRL_ALE;
	if(column != -1) {
		/* column addr: A0~A7 */
		nand_cmdctrl(this, (column >> 0) & 0xff, ctrl);
		/* column addr: A8~A11 */
		nand_cmdctrl(this, (column >> 8) & 0x0f, ctrl);
	}

	if(page != -1) {
		/* row addr: A12~A19 */
		nand_cmdctrl(this, (page >> 0) & 0xff, ctrl);
		/* row addr: A20~A27 */
		nand_cmdctrl(this, (page >> 8) & 0xff, ctrl);
		/* row addr: A28 */
		nand_cmdctrl(this, (page >> 8) & 0x01, ctrl);
	}

	switch(cmd) {
	case NAND_CMD_PAGEPROG:
	case NAND_CMD_ERASE1:
	case NAND_CMD_ERASE2:
	case NAND_CMD_SEQIN:
	case NAND_CMD_STATUS:
		return;

	case NAND_CMD_RESET:
		udelay(5); /* 5us */
		nand_cmdctrl(this, NAND_CMD_STATUS, NAND_CTRL_CLE);
		while(readb(this->IO_NFDATA) & NAND_STATUS_READY);
			;
		break;

	default:
		ndelay(40); /* about 100ns */
		while(readb(&this->ctrl_regs->nfstat) & S3C2440_NFSTATUS_READY)
			;
	}
}

static int nand_erase(struct nand_info *this, u32 addr, int len)
{
	int ret;
	int page, pages_per_block;

	/* addr and len aligned ? */
	if(addr & ((1 << this->erase_shift) - 1)) {
		NF_DEBUG("%s() unaligned address.\n", __func__);
		ret = -EINVAL;
	}

	if(len & ((1 << this->erase_shift) - 1)) {
		NF_DEBUG("%s() length not block aligned.\n", __func__);
		ret = -EINVAL;
	}

	/* chip select */
	nand_cmdctrl(this, NAND_CMD_NONE, NAND_CTRL_CHANGE | NAND_nCE);

	/* is it write protected ? */
	if(check_chip_status(this) & NAND_STATUS_WP) {
		NF_DEBUG("nand_erase: device is write protected!!!\n");
		goto erase_exit;
	}

	page = (int)(addr >> this->page_shift);
	pages_per_block = 1 << (this->erase_shift - this->page_shift);

	do {
		nand_command(this, NAND_CMD_ERASE1, -1, page);
		nand_command(this, NAND_CMD_ERASE2, -1, -1);

		nand_cmdctrl(this, NAND_CMD_STATUS, NAND_CTRL_CLE);
		while(readb(this->IO_NFDATA) & NAND_STATUS_READY)
			;

		if(readb(this->IO_NFDATA) & NAND_STATUS_FAIL) {
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
	nand_cmdctrl(this, NAND_CMD_NONE, NAND_CTRL_CHANGE);

	return (ret == NAND_ERASE_DONE) ? 0 : -EIO;
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

		nand_cmdctrl(this, NAND_CMD_READ0, NAND_CTRL_CLE);

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

static inline void nand_write_page_hwecc(struct nand_info *this, const char *buf, int page)
{
	/*TODO: ecc check */
	nand_write_buf(this, buf, this->writesize);
}

static int nand_do_write_ops(struct nand_info *this, u32 to, struct nand_ops *ops)
{
	int page, column, bytes;
	int writelen;
	int res;

	writelen = ops->len;

	ops->retlen = 0;
	if(!writelen)
		return 0;

	/* select chip */
	nand_cmdctrl(this, NAND_CMD_NONE, NAND_CTRL_CHANGE | NAND_nCE);

	/* is chip write protected ? */
	if(check_chip_status(this) & NAND_STATUS_WP) {
		NF_DEBUG("%s: write protected!\n", __func__);
		goto write_exit;
	}

	column = to & (this->writesize - 1);

	page = to >> this->page_shift;
	page &= this->page_mask;

	do {
		/* pagebuff used for subpage write */
		char pagebuff[NAND_PAGE_SIZE];
		char *wbuf = ops->databuf;
		bytes = this->writesize;

		/* Partial page write ? */
		if(column || writelen < (this->writesize -1)) {
			memset(pagebuff, 0xFF, this->writesize);
			bytes = min(bytes - column, writelen);
			memcpy(&pagebuff[column], ops->databuf, bytes);
			wbuf = pagebuff;
		}

		nand_command(this, NAND_CMD_SEQIN, 0x00, page);
		this->ecc.write_page(this, wbuf, page);
		nand_command(this, NAND_CMD_PAGEPROG, -1, -1);

		/* device ready ? */
		while(check_chip_status(this) & NAND_STATUS_READY)
			;

		if(check_chip_status(this) & NAND_STATUS_FAIL) {
			NF_DEBUG("%s: write page failed, page 0x%x.\n", __func__, page);
			goto write_exit;
		}

		writelen -= bytes;
		wbuf += bytes;
		page++;

		/* cross chip ? */
		if(writelen && !(page & this->page_mask)) {
			NF_DEBUG("%s() cross a chip boundary\n", __func__);
			break;
		}
	} while ((writelen > 0) && (page & this->page_mask));

	res = NAND_WRITE_DONE;
	ops->retlen = ops->len - writelen;

write_exit:
	/* deselect chip */
	nand_cmdctrl(this, NAND_CMD_NONE, NAND_CTRL_CHANGE);
	return (res == NAND_WRITE_DONE) ? 0 : -EIO;
}

static int nand_write(struct nand_info *this, u32 to, size_t len,
		size_t *retlen, const char *buf)
{
	int ret;
	struct nand_ops ops;

	if(((unsigned)to + len) > this->nand_size)
		return -EINVAL;

	if(!len)
		return -EINVAL;

	ops.len = len;
	ops.databuf = (char *)buf;
	ret = nand_do_write_ops(this, to, &ops);

	*retlen = ops.retlen;

	return ret;
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
	nf_info.IO_NFDATA = (u32)&regs->nfdata;
	nf_info.ctrl_regs = regs;

	/*
	 * column address: A0 ~ A11
	 * page address: A12 ~ A28
	 * block address: A18 ~ A28
	 * page size: 2048 (+ 64 oob)
	 * block size: 64 * pages
	 */
	nf_info.nand_size = 0x420000; /* 2048*(2048 + 64) */
	nf_info.page_shift = 12;
	nf_info.page_mask = 0x1FF; /* bit0 ~ bit16, i.e. A12 ~ A28 */
	nf_info.erase_shift = 18;
	nf_info.writesize = 2048;
	//nf_info.oobsize = 2048 >> 5;

	nf_info.write = nand_write;
	nf_info.erase = nand_erase;
	/* ECC functions */
	nf_info.ecc.write_page = nand_write_page_hwecc;

	nandhw_init(&nf_info);
	NF_DEBUG("The end of nand init.\n");
}
