#include <stdint.h>
#include "kcdk_hash.h"
#include "kcdk_job.h"
#include "kcdk_packet.h"

const uint32_t kcdk_hash_sha1_constants[5] = { 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0 };
const uint32_t kcdk_hash_k_constants[4] = { 0x5A827999, 0x6ED9EBA1, 0x8F1BBCDC, 0xCA62C1D6 };

void kcdk_hash_prepare(kcdk_job_context_t *job_ctx) {
    for(
        uint8_t i = (KCDK_PACKET_LENGTH_OLD_HASH / sizeof(job_ctx->to_hash[0]));
        i < ((sizeof(job_ctx->to_hash) / sizeof(job_ctx->to_hash[0])) - (KCDK_PACKET_LENGTH_OLD_HASH / sizeof(job_ctx->to_hash[0])));
        i++
    ) job_ctx->to_hash[i] = 0;
    uint8_t nonce_length = 0;
    for(uint32_t i = job_ctx->nonce; i > 0; i /= 10) nonce_length++;
    if(nonce_length == 0) nonce_length++;

    uint32_t divider = 1;
    for(int16_t i = (nonce_length - 1); i >= 0; i--) {
        job_ctx->to_hash[KCDK_HASH_OFFSET_NONCE + (i / sizeof(job_ctx->to_hash[0]))] |= ((((job_ctx->nonce / divider) % 10) + '0') << (((sizeof(job_ctx->to_hash[0]) - 1) - (i % sizeof(job_ctx->to_hash[0]))) * 8));
        divider *= 10;
    }
    job_ctx->to_hash[KCDK_HASH_OFFSET_NONCE + (nonce_length / sizeof(job_ctx->to_hash[0]))] |= (0x80 << (((sizeof(job_ctx->to_hash[0]) - 1) - (nonce_length % sizeof(job_ctx->to_hash[0]))) * 8));
    job_ctx->to_hash[15] = ((KCDK_HASH_LENGTH_OLD_HASH * sizeof(job_ctx->to_hash[0])) + nonce_length) * 8;
}

void kcdk_hash_sha1(kcdk_job_context_t *job_ctx) {
    for(int i = 0; i < 5; i++) job_ctx->h[i] = kcdk_hash_sha1_constants[i];

    uint32_t a, b, c, d, e, f, k;

    uint32_t temp = 0;
    for(uint8_t i = 16; i < 80; i++) {
        temp = (job_ctx->to_hash[i - 3] ^ job_ctx->to_hash[i - 8] ^ job_ctx->to_hash[i - 14] ^ job_ctx->to_hash[i - 16]);
        job_ctx->to_hash[i] = ROTL32(temp, 1);
    }

    a = job_ctx->h[0];
    b = job_ctx->h[1];
    c = job_ctx->h[2];
    d = job_ctx->h[3];
    e = job_ctx->h[4];

    for(uint8_t i = 0; i < 80; i++) {
        if(i < 20) {
            f = ((b & c) | ((~b) & d));
            k = kcdk_hash_k_constants[0];
        } else if(i < 40) {
            f = (b ^ c ^ d);
            k = kcdk_hash_k_constants[1];
        } else if(i < 60) {
            f = ((b & c) | (b & d) | (c & d));
            k = kcdk_hash_k_constants[2];
        } else if(i < 80) {
            f = (b ^ c ^ d);
            k = kcdk_hash_k_constants[3];
        }

        temp = ROTL32(a, 5) + f + e + k + job_ctx->to_hash[i];
        e = d;
        d = c;
        c = ROTL32(b, 30);
        b = a;
        a = temp;
    }

    job_ctx->h[0] += a;
    job_ctx->h[1] += b;
    job_ctx->h[2] += c;
    job_ctx->h[3] += d;
    job_ctx->h[4] += e;
}
