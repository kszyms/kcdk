#include "ch32v003fun.h"
#include <stdint.h>
#include "kcdk_config.h"
#include "kcdk_usart.h"

void kcdk_usart_init() {
    RCC->APB2PCENR |= RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOD | RCC_APB2Periph_USART1;

    #if(KCDK_USART_PIN_SWAP == 1)
        AFIO->PCFR1 = ((AFIO->PCFR1 & (~AFIO_PCFR1_USART1_REMAP)) | AFIO_PCFR1_USART1_REMAP_1);

        GPIOD->CFGLR &= ~(0xF << (4 * 5));
        GPIOD->CFGLR |= (GPIO_CNF_IN_FLOATING << (4 * 5));

        GPIOD->CFGLR &= ~(0xF << (4 * 6));
        GPIOD->CFGLR |= ((GPIO_CNF_OUT_OD_AF | GPIO_Speed_10MHz) << (4 * 6));
    #else
        AFIO->PCFR1 &= ((~AFIO_PCFR1_USART1_REMAP_1) & (~AFIO_PCFR1_USART1_REMAP));

        GPIOD->CFGLR &= ~(0xF << (4 * 5));
        GPIOD->CFGLR |= ((GPIO_CNF_OUT_OD_AF | GPIO_Speed_10MHz) << (4 * 5));

        GPIOD->CFGLR &= ~(0xF << (4 * 6));
        GPIOD->CFGLR |= (GPIO_CNF_IN_FLOATING << (4 * 6));
    #endif

    USART1->CTLR1 = USART_WordLength_8b | USART_Parity_No | USART_Mode_Tx | USART_Mode_Rx | USART_CTLR1_RXNEIE;
    USART1->CTLR2 = USART_StopBits_1;
    USART1->CTLR3 = 0;

    USART1->BRR = ((FUNCONF_SYSTEM_CORE_CLOCK + (KCDK_USART_BAUD_RATE / 2)) / KCDK_USART_BAUD_RATE);

    USART1->CTLR1 |= CTLR1_UE_Set;

    NVIC_EnableIRQ(USART1_IRQn);
}

void kcdk_usart_write(uint8_t *message, uint8_t length) {
    for(uint8_t i = 0; i < length; i++) {
        while(!(USART1->STATR & USART_STATR_TXE));
        USART1->DATAR = (*(message + i));
    }
}

uint8_t kcdk_usart_read_uint8(kcdk_usart_context_t *usart_ctx) {
    if(usart_ctx->available) {
        uint8_t val = usart_ctx->buffer[usart_ctx->tail];
        usart_ctx->tail = ((usart_ctx->tail + 1) % KCDK_USART_BUFFER_SIZE);
        usart_ctx->available--;

        return val;
    }

    return 0;
}

uint16_t kcdk_usart_read_uint16(kcdk_usart_context_t *usart_ctx) {
    if(usart_ctx->available >= sizeof(uint16_t)) {
        uint16_t val = 0;
        for(int i = 0; i < sizeof(uint16_t); i++) {
            val |= (((uint16_t)(kcdk_usart_read_uint8(usart_ctx))) << (((sizeof(uint16_t) - 1) - i) * 8));
        }

        return val;
    }

    return 0;
}

uint32_t kcdk_usart_read_uint32(kcdk_usart_context_t *usart_ctx) {
    if(usart_ctx->available >= sizeof(uint32_t)) {
        uint32_t val = 0;
        for(int i = 0; i < sizeof(uint32_t); i++) {
            val |= (((uint32_t)(kcdk_usart_read_uint8(usart_ctx))) << (((sizeof(uint32_t) - 1) - i) * 8));
        }

        return val;
    }

    return 0;
}

uint64_t kcdk_usart_read_uint64(kcdk_usart_context_t *usart_ctx) {
    if(usart_ctx->available >= sizeof(uint64_t)) {
        uint64_t val = 0;
        for(int i = 0; i < sizeof(uint64_t); i++) {
            val |= (((uint64_t)(kcdk_usart_read_uint8(usart_ctx))) << (((sizeof(uint64_t) - 1) - i) * 8));
        }

        return val;
    }

    return 0;
}

uint8_t kcdk_usart_peek_uint8(kcdk_usart_context_t *usart_ctx, uint8_t offset) {
    if(offset < usart_ctx->available) {
        uint8_t pointer = (uint8_t)(((uint16_t)usart_ctx->tail + (uint16_t)offset) % KCDK_USART_BUFFER_SIZE);
        uint8_t val = usart_ctx->buffer[pointer];
        
        return val;
    }

    return 0;
}

uint16_t kcdk_usart_peek_uint16(kcdk_usart_context_t *usart_ctx, uint8_t offset) {
    if((offset + (sizeof(uint16_t) - 1)) < usart_ctx->available) {
        uint16_t val = 0;
        for(int i = 0; i < sizeof(uint16_t); i++) {
            val |= (((uint16_t)(kcdk_usart_peek_uint8(usart_ctx, offset + i))) << (((sizeof(uint16_t) - 1) - i) * 8));
        }

        return val;
    }

    return 0;
}

uint32_t kcdk_usart_peek_uint32(kcdk_usart_context_t *usart_ctx, uint8_t offset) {
    if((offset + (sizeof(uint32_t) - 1)) < usart_ctx->available) {
        uint32_t val = 0;
        for(int i = 0; i < sizeof(uint32_t); i++) {
            val |= (((uint32_t)(kcdk_usart_peek_uint8(usart_ctx, offset + i))) << (((sizeof(uint32_t) - 1) - i) * 8));
        }

        return val;
    }

    return 0;
}

uint64_t kcdk_usart_peek_uint64(kcdk_usart_context_t *usart_ctx, uint8_t offset) {
    if((offset + (sizeof(uint64_t) - 1)) < usart_ctx->available) {
        uint64_t val = 0;
        for(int i = 0; i < sizeof(uint64_t); i++) {
            val |= (((uint64_t)(kcdk_usart_peek_uint8(usart_ctx, offset + i))) << (((sizeof(uint64_t) - 1) - i) * 8));
        }

        return val;
    }

    return 0;
}

void kcdk_usart_discard(kcdk_usart_context_t *usart_ctx, uint8_t amount) {
    if(amount > usart_ctx->available) amount = usart_ctx->available;
    usart_ctx->tail = (uint8_t)(((uint16_t)usart_ctx->tail + (uint16_t)amount) % KCDK_USART_BUFFER_SIZE);
    usart_ctx->available -= amount;
}

void kcdk_usart_flush(kcdk_usart_context_t *usart_ctx) {
    usart_ctx->available = 0;
    usart_ctx->head = 0;
    usart_ctx->tail = 0;
}
