#ifndef _STM322XG_EXTSRAM_H
#define _STM322XG_EXTSRAM_H 1

#include <stdint.h>

/* FSMC Bank 1 NOR/SRAM2 */

#define BANK1_SRAM2_ADDR  ((uint32_t)0x64000000)

void SRAMconfigure(void);

/* 2 MiB RAMDISK, logical block size, logical block count */

#define LB_SIZE   512
#define LB_COUNT  4096

typedef uint8_t ram_disk_sector_t[LB_SIZE];

#define RAMDISK  ((ram_disk_sector_t *)(BANK1_SRAM2_ADDR))

#endif
