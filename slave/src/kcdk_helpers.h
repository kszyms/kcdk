#ifndef KCDK_HELPERS_H
#define KCDK_HELPERS_H

#include <stdint.h>

#define KCDK_HELPERS_CHIP_ID __builtin_bswap64((*(uint64_t *)0x1FFFF7E8))

uint32_t kcdk_helpers_get_timeout_cycles(uint32_t ms);

#endif
