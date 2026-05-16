#include <stdint.h>
#include "kcdk_packet.h"
#include "kcdk_helpers.h"
#include "kcdk_job.h"
#include "kcdk_usart.h"

const uint8_t kcdk_packet_checksum_constants[256] = { 0x00, 0xD5, 0x7F, 0xAA, 0xFE, 0x2B, 0x81, 0x54, 0x29, 0xFC, 0x56, 0x83, 0xD7, 0x02, 0xA8, 0x7D, 0x52, 0x87, 0x2D, 0xF8, 0xAC, 0x79, 0xD3, 0x06, 0x7B, 0xAE, 0x04, 0xD1, 0x85, 0x50, 0xFA, 0x2F, 0xA4, 0x71, 0xDB, 0x0E, 0x5A, 0x8F, 0x25, 0xF0, 0x8D, 0x58, 0xF2, 0x27, 0x73, 0xA6, 0x0C, 0xD9, 0xF6, 0x23, 0x89, 0x5C, 0x08, 0xDD, 0x77, 0xA2, 0xDF, 0x0A, 0xA0, 0x75, 0x21, 0xF4, 0x5E, 0x8B, 0x9D, 0x48, 0xE2, 0x37, 0x63, 0xB6, 0x1C, 0xC9, 0xB4, 0x61, 0xCB, 0x1E, 0x4A, 0x9F, 0x35, 0xE0, 0xCF, 0x1A, 0xB0, 0x65, 0x31, 0xE4, 0x4E, 0x9B, 0xE6, 0x33, 0x99, 0x4C, 0x18, 0xCD, 0x67, 0xB2, 0x39, 0xEC, 0x46, 0x93, 0xC7, 0x12, 0xB8, 0x6D, 0x10, 0xC5, 0x6F, 0xBA, 0xEE, 0x3B, 0x91, 0x44, 0x6B, 0xBE, 0x14, 0xC1, 0x95, 0x40, 0xEA, 0x3F, 0x42, 0x97, 0x3D, 0xE8, 0xBC, 0x69, 0xC3, 0x16, 0xEF, 0x3A, 0x90, 0x45, 0x11, 0xC4, 0x6E, 0xBB, 0xC6, 0x13, 0xB9, 0x6C, 0x38, 0xED, 0x47, 0x92, 0xBD, 0x68, 0xC2, 0x17, 0x43, 0x96, 0x3C, 0xE9, 0x94, 0x41, 0xEB, 0x3E, 0x6A, 0xBF, 0x15, 0xC0, 0x4B, 0x9E, 0x34, 0xE1, 0xB5, 0x60, 0xCA, 0x1F, 0x62, 0xB7, 0x1D, 0xC8, 0x9C, 0x49, 0xE3, 0x36, 0x19, 0xCC, 0x66, 0xB3, 0xE7, 0x32, 0x98, 0x4D, 0x30, 0xE5, 0x4F, 0x9A, 0xCE, 0x1B, 0xB1, 0x64, 0x72, 0xA7, 0x0D, 0xD8, 0x8C, 0x59, 0xF3, 0x26, 0x5B, 0x8E, 0x24, 0xF1, 0xA5, 0x70, 0xDA, 0x0F, 0x20, 0xF5, 0x5F, 0x8A, 0xDE, 0x0B, 0xA1, 0x74, 0x09, 0xDC, 0x76, 0xA3, 0xF7, 0x22, 0x88, 0x5D, 0xD6, 0x03, 0xA9, 0x7C, 0x28, 0xFD, 0x57, 0x82, 0xFF, 0x2A, 0x80, 0x55, 0x01, 0xD4, 0x7E, 0xAB, 0x84, 0x51, 0xFB, 0x2E, 0x7A, 0xAF, 0x05, 0xD0, 0xAD, 0x78, 0xD2, 0x07, 0x53, 0x86, 0x2C, 0xF9 };

uint8_t kcdk_packet_find_sync(kcdk_usart_context_t *usart_ctx) {
    while(usart_ctx->available >= KCDK_PACKET_LENGTH_SYNC) {
        if(
            kcdk_usart_peek_uint8(usart_ctx, KCDK_PACKET_OFFSET_SYNC_1ST) == KCDK_PACKET_VALUE_SYNC_MASTER_1ST &&
            kcdk_usart_peek_uint8(usart_ctx, KCDK_PACKET_OFFSET_SYNC_2ND) == KCDK_PACKET_VALUE_SYNC_MASTER_2ND
        ) return 1;
        kcdk_usart_discard(usart_ctx, 1);
    }

    return 0;
}

uint8_t kcdk_packet_get_header(kcdk_usart_context_t *usart_ctx) {

    return kcdk_usart_peek_uint8(usart_ctx, KCDK_PACKET_OFFSET_HEADER);
}

uint8_t kcdk_packet_check_destination(kcdk_usart_context_t *usart_ctx) {
    uint64_t read_dst = kcdk_usart_peek_uint64(usart_ctx, KCDK_PACKET_OFFSET_DESTINATION);

    return ((read_dst ^ KCDK_HELPERS_CHIP_ID) == 0);
}

void kcdk_packet_parse_job(kcdk_usart_context_t *usart_ctx, kcdk_job_context_t *job_ctx) {
    for(int i = 0; i < (KCDK_PACKET_LENGTH_OLD_HASH / sizeof(job_ctx->to_hash[0])); i++)
        job_ctx->to_hash[i] = kcdk_usart_peek_uint32(usart_ctx, (KCDK_PACKET_OFFSET_OLD_HASH + (i * sizeof(uint32_t))));
    for(int i = 0; i < (KCDK_PACKET_LENGTH_EXPECTED_HASH / sizeof(job_ctx->expected[0])); i++)
        job_ctx->expected[i] = kcdk_usart_peek_uint32(usart_ctx, (KCDK_PACKET_OFFSET_EXPECTED_HASH + (i * sizeof(uint32_t))));
    job_ctx->max_nonce = ((kcdk_usart_peek_uint32(usart_ctx, KCDK_PACKET_OFFSET_DIFFICULTY) * 1000) + 1);
    job_ctx->nonce = 0;
}

void kcdk_packet_prepare(kcdk_job_context_t *job_ctx) {
    job_ctx->last_message[KCDK_PACKET_OFFSET_SYNC_1ST] = KCDK_PACKET_VALUE_SYNC_SLAVE_1ST;
    job_ctx->last_message[KCDK_PACKET_OFFSET_SYNC_2ND] = KCDK_PACKET_VALUE_SYNC_SLAVE_2ND;
    for(int i = 0; i < KCDK_PACKET_LENGTH_SOURCE; i++) job_ctx->last_message[KCDK_PACKET_OFFSET_SOURCE + i] = (uint8_t)((KCDK_HELPERS_CHIP_ID >> ((KCDK_PACKET_LENGTH_SOURCE - 1) - i) * 8) & 0xFF);
}

void kcdk_packet_checksum_calculate(kcdk_job_context_t *job_ctx) {
    uint8_t checksum = 0x00;
    job_ctx->last_message[KCDK_PACKET_OFFSET_CHECKSUM] = KCDK_PACKET_VALUE_CHECKSUM_PRE;
    for(int i = 0; i < job_ctx->last_message_len; i++) checksum = kcdk_packet_checksum_constants[checksum ^ job_ctx->last_message[i]];
    job_ctx->last_message[KCDK_PACKET_OFFSET_CHECKSUM] = checksum;
}

uint8_t kcdk_packet_checksum_validate(kcdk_usart_context_t *usart_ctx, uint8_t length) {
    uint8_t checksum = 0x00;
    for(int i = 0; i < KCDK_PACKET_OFFSET_CHECKSUM; i++) checksum = kcdk_packet_checksum_constants[checksum ^ kcdk_usart_peek_uint8(usart_ctx, i)];
    checksum = kcdk_packet_checksum_constants[checksum ^ KCDK_PACKET_VALUE_CHECKSUM_PRE];
    for(int i = (KCDK_PACKET_OFFSET_CHECKSUM + 1); i < length; i++) checksum = kcdk_packet_checksum_constants[checksum ^ kcdk_usart_peek_uint8(usart_ctx, i)];

    return (kcdk_usart_peek_uint8(usart_ctx, KCDK_PACKET_OFFSET_CHECKSUM) == checksum);
}

void kcdk_packet_send_ack(kcdk_job_context_t *job_ctx) {
    job_ctx->last_message_len = KCDK_PACKET_LENGTH_ACK;
    kcdk_packet_prepare(job_ctx);
    job_ctx->last_message[KCDK_PACKET_OFFSET_HEADER] = KCDK_PACKET_VALUE_HEADER_ACK;
    kcdk_packet_checksum_calculate(job_ctx);
    kcdk_usart_write(job_ctx->last_message, job_ctx->last_message_len);
}

void kcdk_packet_send_idn(kcdk_job_context_t *job_ctx) {
    job_ctx->last_message_len = KCDK_PACKET_LENGTH_IDN;
    kcdk_packet_prepare(job_ctx);
    job_ctx->last_message[KCDK_PACKET_OFFSET_HEADER] = KCDK_PACKET_VALUE_HEADER_IDN;
    kcdk_packet_checksum_calculate(job_ctx);
    kcdk_usart_write(job_ctx->last_message, job_ctx->last_message_len);
}

void kcdk_packet_send_jrq(kcdk_job_context_t *job_ctx) {
    job_ctx->last_message_len = KCDK_PACKET_LENGTH_JRQ;
    kcdk_packet_prepare(job_ctx);
    job_ctx->last_message[KCDK_PACKET_OFFSET_HEADER] = KCDK_PACKET_VALUE_HEADER_JRQ;
    kcdk_packet_checksum_calculate(job_ctx);
    kcdk_usart_write(job_ctx->last_message, job_ctx->last_message_len);
}

void kcdk_packet_send_jrs(kcdk_job_context_t *job_ctx) {
    job_ctx->last_message_len = KCDK_PACKET_LENGTH_JRS;
    kcdk_packet_prepare(job_ctx);
    job_ctx->last_message[KCDK_PACKET_OFFSET_HEADER] = KCDK_PACKET_VALUE_HEADER_JRS;
    for(int i = 0; i < KCDK_PACKET_LENGTH_NONCE; i++) job_ctx->last_message[KCDK_PACKET_OFFSET_NONCE + i] = ((uint8_t)((job_ctx->nonce >> (8 * i)) & 0xFF));
    kcdk_packet_checksum_calculate(job_ctx);
    kcdk_usart_write(job_ctx->last_message, job_ctx->last_message_len);
}

void kcdk_packet_resend(kcdk_job_context_t *job_ctx) {
    kcdk_usart_write(job_ctx->last_message, job_ctx->last_message_len);
}
