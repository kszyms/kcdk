#include "ch32v003fun.h"
#include "kcdk_usart.h"
#include "kcdk_config.h"

void USART1_IRQHandler(void) __attribute__((interrupt));

kcdk_usart_context my_context = { 0 };

int main() {
    SystemInit();
    kcdk_usart_init();

    uint8_t msg[7] = { 0 };
    for(;;) {
        if(my_context.available) {
            msg[0] = ((my_context.available / 100) + '0');
            msg[1] = (((my_context.available / 10) % 10) + '0');
            msg[2] = ((my_context.available % 10) + '0');
            msg[3] = ' ';
            msg[4] = kcdk_usart_read(&my_context);
            msg[5] = '\n';
            msg[6] = '\r';
            kcdk_usart_write(msg, 7);
        }
    }

    return 0;
}

void USART1_IRQHandler(void) {
    if(USART1->STATR & USART_STATR_RXNE) {
        my_context.buffer[my_context.head] = ((uint8_t)(USART1->DATAR & 0xFF));
        my_context.head = ((my_context.head + 1) % KCDK_USART_BUFFER_SIZE);
        if(my_context.available < KCDK_USART_BUFFER_SIZE) my_context.available++;
        else my_context.tail = ((my_context.tail + 1) % KCDK_USART_BUFFER_SIZE);
    }
}
