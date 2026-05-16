#include "ch32v003fun.h"
#include <stdint.h>
#include "kcdk_helpers.h"

uint32_t kcdk_helpers_get_timeout_cycles(uint32_t ms) {
    uint8_t divisor = ((SysTick->CTLR & SYSTICK_CTLR_STCLK) ? 1 : 8);
    uint32_t cycles_per_ms = ((FUNCONF_SYSTEM_CORE_CLOCK / 1000) / divisor);

    return cycles_per_ms * ms;
}
