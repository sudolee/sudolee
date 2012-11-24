#ifndef __NAND_H_
#define __NAND_H_

#include "type.h"
#include "io.h"
#include "s3c2440.h"

#define NAND_CTRL_ENTRY  0x4e000000

#define NFCONF 0x1210
#define NFCONT 0x1210

#define NFCMMD  0x0

#define NF_CE (1 << 22)
#define NF_RE (1 << 20)
#define NF_WE (1 << 19)
#define NF_ALE (1 << 18)
#define NF_CLE (1 << 17)

#define NF_RNB (1 << 0)

#define NF_MAX_ADDRESS ((0x1 << 28) - 1)

/*
 * nand flash commands
 */
#define NF_CMD_RESET 0xff
#define NF_CMD_READ0 0x00
#define NF_CMD_READ1 0x30
#define NF_CMD_PGPROG0 0x80
#define NF_CMD_PGPROG1 0x10
#define NF_CMD_BLKES0 0x60
#define NF_CMD_BLKES1 0xd0
#define NF_CMD_RDID 0x90
#define NF_CMD_RDSTATUS 0x70

#endif /* __NAND_H_ */
