#ifndef KCDK_USART_H
#define KCDK_USART_H

#include <stdint.h>

#define KCDK_USART_BUFFER_SIZE 100

typedef struct {
    volatile uint8_t buffer[KCDK_USART_BUFFER_SIZE];
    volatile uint8_t available;
    volatile uint8_t head;
    volatile uint8_t tail;
} kcdk_usart_context_t;

void kcdk_usart_init();
void kcdk_usart_write(uint8_t *message, uint8_t length);
uint8_t kcdk_usart_read_uint8(kcdk_usart_context_t *usart_ctx);
uint16_t kcdk_usart_read_uint16(kcdk_usart_context_t *usart_ctx);
uint32_t kcdk_usart_read_uint32(kcdk_usart_context_t *usart_ctx);
uint64_t kcdk_usart_read_uint64(kcdk_usart_context_t *usart_ctx);
uint8_t kcdk_usart_peek_uint8(kcdk_usart_context_t *usart_ctx, uint8_t offset);
uint16_t kcdk_usart_peek_uint16(kcdk_usart_context_t *usart_ctx, uint8_t offset);
uint32_t kcdk_usart_peek_uint32(kcdk_usart_context_t *usart_ctx, uint8_t offset);
uint64_t kcdk_usart_peek_uint64(kcdk_usart_context_t *usart_ctx, uint8_t offset);
void kcdk_usart_discard(kcdk_usart_context_t *usart_ctx, uint8_t amount);
void kcdk_usart_flush(kcdk_usart_context_t *usart_ctx);

#endif
