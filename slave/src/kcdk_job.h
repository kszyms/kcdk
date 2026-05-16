#ifndef KCDK_JOB_H
#define KCDK_JOB_H

#include <stdint.h>

enum kcdk_job_status {
    KCDK_JOB_STATUS_IDN = 0,
    KCDK_JOB_STATUS_JRQ,
    KCDK_JOB_STATUS_JRS,
    KCDK_JOB_STATUS_JOB_WAITING,
    KCDK_JOB_STATUS_ACK_WAITING,
    KCDK_JOB_STATUS_WORKING
};

typedef struct {
    uint32_t expected[5];
    uint32_t h[5];
    uint32_t to_hash[80];
    uint32_t nonce;
    uint32_t max_nonce;
    enum kcdk_job_status status;
    enum kcdk_job_status next_status;
    uint8_t last_message[20];
    uint8_t last_message_len;
} kcdk_job_context_t;

#endif
