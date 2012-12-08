#ifndef __MEMORY_H_
#define __MEMORY_H_

#include "type.h"
#include "io.h"
#include "s3c2440.h"

/* memory controller entry */
#define MEMORY_ENTRY 0x48000000

void memory_init(void);

#endif /* __MEMORY_H_ */
