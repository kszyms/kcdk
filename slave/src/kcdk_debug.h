#ifndef KCDK_DEBUG_H
#define KCDK_DEBUG_H

#include <stdint.h>
#include "kcdk_usart.h"

void kcdk_debug_usart_write(uint8_t *messge, uint8_t length);
void kcdk_debug_usart_set(kcdk_usart_context_t *usart_ctx, uint8_t *message, uint8_t length);

#endif
