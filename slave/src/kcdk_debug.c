#include <stdint.h>
#include "kcdk_debug.h"
#include "kcdk_usart.h"

void kcdk_debug_usart_write(uint8_t *message, uint8_t length) {
    kcdk_usart_write((uint8_t *)"\n\r", 2);
    uint8_t temp[2] = { 0 };
    for(int i = 0; i < length; i++) {
        temp[0] = ((message[i] >> 4) & 0x0F);
        if(temp[0] > 9) temp[0] += ('A' - 10);
        else temp[0] += '0';
        temp[1] = (message[i] & 0x0F);
        if(temp[1] > 9) temp[1] += ('A' - 10);
        else temp[1] += '0';
        kcdk_usart_write(temp, 2);
    }
    kcdk_usart_write((uint8_t *)"\n\r", 2);
}

void kcdk_debug_usart_set(kcdk_usart_context_t *usart_ctx, uint8_t *message, uint8_t length) {
    for(int i = 0; i < length; i++) {
        usart_ctx->buffer[i] = message[i];
    }
    usart_ctx->available = length;
    usart_ctx->head = length;
    usart_ctx->tail = 0;
}
