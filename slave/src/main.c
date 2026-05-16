#include "ch32v003fun.h"
#include <stdint.h>
#include "kcdk_config.h"
#include "kcdk_hash.h"
#include "kcdk_helpers.h"
#include "kcdk_job.h"
#include "kcdk_packet.h"
#include "kcdk_usart.h"

void USART1_IRQHandler(void) __attribute__((interrupt));

kcdk_usart_context_t kcdk_usart_context = { 0 };

int main() {
    SystemInit();
    kcdk_usart_init();
    
    uint32_t kcdk_timeout_send_cycle = 0;
    uint32_t kcdk_timeout_timeout_cycles = (kcdk_helpers_get_timeout_cycles(500) + ((uint32_t)(KCDK_HELPERS_CHIP_ID & 0xFFFFF)));
    kcdk_job_context_t kcdk_job_context = { 0 };
    kcdk_job_context.status = KCDK_JOB_STATUS_IDN;
    
    for(;;) {
        switch (kcdk_job_context.status) {
            case KCDK_JOB_STATUS_IDN:
                kcdk_packet_send_idn(&kcdk_job_context);
                kcdk_usart_flush(&kcdk_usart_context);
                kcdk_timeout_send_cycle = SysTick->CNT;
                kcdk_job_context.status = KCDK_JOB_STATUS_ACK_WAITING;
                kcdk_job_context.next_status = KCDK_JOB_STATUS_JRQ;

                break;
            
            case KCDK_JOB_STATUS_JRQ:
                kcdk_packet_send_jrq(&kcdk_job_context);
                kcdk_usart_flush(&kcdk_usart_context);
                kcdk_timeout_send_cycle = SysTick->CNT;
                kcdk_job_context.status = KCDK_JOB_STATUS_ACK_WAITING;
                kcdk_job_context.next_status = KCDK_JOB_STATUS_JOB_WAITING;

                break;

            case KCDK_JOB_STATUS_JOB_WAITING:
                if(
                    (!(kcdk_packet_find_sync(&kcdk_usart_context))) ||
                    (kcdk_usart_context.available < KCDK_PACKET_LENGTH_JOB)
                ) break;
                if(
                    (!(kcdk_packet_check_destination(&kcdk_usart_context))) ||
                    (kcdk_packet_get_header(&kcdk_usart_context) != KCDK_PACKET_VALUE_HEADER_JOB) ||
                    (!(kcdk_packet_checksum_validate(&kcdk_usart_context, KCDK_PACKET_LENGTH_JOB)))
                ) {
                    kcdk_usart_discard(&kcdk_usart_context, 2);
                    break;
                }
                kcdk_packet_parse_job(&kcdk_usart_context, &kcdk_job_context);
                kcdk_packet_send_ack(&kcdk_job_context);
                kcdk_job_context.status = KCDK_JOB_STATUS_WORKING;
                kcdk_usart_discard(&kcdk_usart_context, 2);

                break;

            case KCDK_JOB_STATUS_ACK_WAITING:
                if((SysTick->CNT - kcdk_timeout_send_cycle) > kcdk_timeout_timeout_cycles) {
                    kcdk_packet_resend(&kcdk_job_context);
                    kcdk_timeout_send_cycle = SysTick->CNT;
                }
                if(
                    (!(kcdk_packet_find_sync(&kcdk_usart_context))) ||
                    (kcdk_usart_context.available < KCDK_PACKET_LENGTH_ACK)
                ) break;
                if(
                    (!(kcdk_packet_check_destination(&kcdk_usart_context))) ||
                    (kcdk_packet_get_header(&kcdk_usart_context) != KCDK_PACKET_VALUE_HEADER_ACK) ||
                    (!(kcdk_packet_checksum_validate(&kcdk_usart_context, KCDK_PACKET_LENGTH_ACK)))
                ) {
                    kcdk_usart_discard(&kcdk_usart_context, 2);
                    break;
                }
                kcdk_job_context.status = kcdk_job_context.next_status;
                kcdk_usart_discard(&kcdk_usart_context, 2);

                break;

            case KCDK_JOB_STATUS_WORKING:
                kcdk_hash_prepare(&kcdk_job_context);
                kcdk_hash_sha1(&kcdk_job_context);
                kcdk_job_context.status = KCDK_JOB_STATUS_JRS;
                for(int i = 0; i < 5; i++) {
                    if(kcdk_job_context.h[i] != kcdk_job_context.expected[i]) {
                        kcdk_job_context.status = KCDK_JOB_STATUS_WORKING;
                        kcdk_job_context.nonce++;
                        break;
                    }
                }
                if(kcdk_job_context.nonce > kcdk_job_context.max_nonce) kcdk_job_context.status = KCDK_JOB_STATUS_JRS;

                break;

            case KCDK_JOB_STATUS_JRS:
                kcdk_packet_send_jrs(&kcdk_job_context);
                kcdk_usart_flush(&kcdk_usart_context);
                kcdk_timeout_send_cycle = SysTick->CNT;
                kcdk_job_context.status = KCDK_JOB_STATUS_ACK_WAITING;
                kcdk_job_context.next_status = KCDK_JOB_STATUS_JRQ;

                break;
                
            default:
                kcdk_job_context.status = KCDK_JOB_STATUS_JRQ;
                break;
        }
    }

    return 0;
}

void USART1_IRQHandler(void) {
    if(USART1->STATR & USART_STATR_RXNE) {
        kcdk_usart_context.buffer[kcdk_usart_context.head] = ((uint8_t)(USART1->DATAR & 0xFF));
        kcdk_usart_context.head = ((kcdk_usart_context.head + 1) % KCDK_USART_BUFFER_SIZE);
        if(kcdk_usart_context.available < KCDK_USART_BUFFER_SIZE) kcdk_usart_context.available++;
        else kcdk_usart_context.tail = ((kcdk_usart_context.tail + 1) % KCDK_USART_BUFFER_SIZE);
    }
}
