#ifndef __NAND_H_
#define __NAND_H_

#include "type.h"
#include "io.h"
#include "s3c2440.h"

#include "print.h"

/*
 * platform relative
 */
#define NAND_CTRL_ENTRY  0x4e000000

#define S3C2440_NFCONT 0x04
#define S3C2440_NFCMD 0x08
#define S3C2440_NFADDR 0x0c

/* bits define */
#define S3C2440_NFSELBIT (1 << 0)
#define S3C2440_NFSTATUS_READY (1 << 0)

/* Size:
 * block size = 2048 * pages
 * page size = (2048 + 64) bytes; 64 bytes OOB area
 *
 */
/* nand_info->writesize */
#define NAND_PAGE_SIZE 2048

//ecc bytes = 512
#define NAND_ECCBYTES (1 << 9)


/* status flags */
#define NAND_ERASE_DONE 0x1
#define NAND_WRITE_DONE 0x2

/* control flags */
#define NAND_CMD_NONE -1
#define NAND_nCE 			(1 << 0)
#define NAND_CLE 			(1 << 1)
#define NAND_ALE 			(1 << 2)
#define NAND_CTRL_CHANGE 	(1 << 3)
#define NAND_CTRL_CLE (NAND_nCE | NAND_CLE)
#define NAND_CTRL_ALE (NAND_nCE | NAND_ALE)

/*
 * Standard NAND flash commands
 */
#define NAND_CMD_READ0      0
#define NAND_CMD_READ1      1
#define NAND_CMD_RNDOUT     5
#define NAND_CMD_PAGEPROG   0x10
#define NAND_CMD_READOOB    0x50
#define NAND_CMD_ERASE1     0x60
#define NAND_CMD_STATUS     0x70
#define NAND_CMD_STATUS_MULTI   0x71
#define NAND_CMD_SEQIN      0x80
#define NAND_CMD_RNDIN      0x85
#define NAND_CMD_READID     0x90
#define NAND_CMD_ERASE2     0xd0
#define NAND_CMD_PARAM      0xec
#define NAND_CMD_GET_FEATURES   0xee
#define NAND_CMD_SET_FEATURES   0xef
#define NAND_CMD_RESET      0xff

#define NAND_CMD_LOCK       0x2a
#define NAND_CMD_UNLOCK1    0x23
#define NAND_CMD_UNLOCK2    0x24

/* status bit define for 70h command */
#define NAND_STATUS_FAIL 	(1 << 0)
#define NAND_STATUS_READY 	(1 << 6)
#define NAND_STATUS_WP 		(1 << 7)

/* exported functions... */
struct nand_info *get_nandinfo(void);

#endif /* __NAND_H_ */
