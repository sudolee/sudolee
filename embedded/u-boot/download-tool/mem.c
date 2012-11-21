#include "s3c2440.h"

/* memory controller entry */
#define MEMORY_ENTRY 0x48000000

static inline struct mem_t *get_mem_base(void)
{
	return (struct mem_t *)MEMORY_ENTRY;
}

void mem_init(void)
{
	struct mem_t *mem_config = get_mem_base();

	/*
	 * BWSCON
	 * bank6: SDRAM
	 * ST6[27] = 0b0, use nWBE[3:0]
	 * WS6[26] = 0b0, disable WAIT
	 * DW6[25:24] = 0b10, 32-bit
	 *
	 * bank0: Nor flash
	 *  this bank config by OM[1:0] pins.
	 */
	writel(&mem_config->bwscon, 0x2212D110);

	writel(&mem_config->bankcon[0], 0x00000F40);
	writel(&mem_config->bankcon[1], 0x00002E50);
	writel(&mem_config->bankcon[2], 0x00002E50);
	writel(&mem_config->bankcon[3], 0x00002E50);
	writel(&mem_config->bankcon[4], 0x00002E50);
	writel(&mem_config->bankcon[5], 0x00002E50);

	/*
	 * BANKCON6 - used for SDRAM
	 * MT[16:15] = 0b11, connect to sdram
	 * Tacs[14:13] = 0b00, address setup time before nGCS
	 * Tcos[12:11] = 0b00, CS setup time before nOE
	 * Tacc[10:8] = 0b010, access cycle
	 * Tcoh[7:6] = default, CS hold time after nOE
	 * Tcah[5:4} = default, adress hold time after nGCS
	 * Trcd[3:2] = 0b01(3clocks), RAS to CAS delay, Page18 specify Trcd=Thclk*n >= 20ns
	 * SCAN[1:0] = 0b01, column address number is 9-bit
	 */
	writel(&mem_config->bankcon[6], 0x00018005);
	writel(&mem_config->bankcon[7], 0x00018005);

	/* 
	 * REFRESH
	 * REFEN[23] = 1, refresh enable
	 * TREFMD[22] = 0, 0 CBR/auto refresh, 1 self refresh
	 * Trp[21:20] = 0b01 , RAS prechange time, 3clocks
	 * Tsrc[19:18] = 0b11, Semi Row cycle time, 7clock, Tsrc = Trp + Trc(60ns)
	 * Refresh counter: =(2^11 - (refresh period)/Hclk + 1) = 995
	 *   refresh period = 64ms/8192 = 7.8125us
	 *   Hclk = Fclk/3 = 135MHz
	 */
	writel(&mem_config->refresh, 0x009c03e3);

	/*
	 * BANKSIZE
	 * BURST_EN[7] = 1, enable burst operation
	 * SCKE_EN[5] = 1, power down mode enable
	 * SCLK_EN[4] = 1
	 * BK76MAP[2:0] = 001, 64MB/64MB
	 */
	writel(&mem_config->banksize, 0xb1);

	/* CAS latency = 3 clocks */
	writel(&mem_config->mrsrb6, 0x30);
	writel(&mem_config->mrsrb7, 0x30);
}
