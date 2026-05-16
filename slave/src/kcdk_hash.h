#ifndef KCDK_HASH_H
#define KCDK_HASH_H

#include <stdint.h>
#include "kcdk_job.h"

#define KCDK_HASH_LENGTH_OLD_HASH 10
#define KCDK_HASH_OFFSET_OLD_HASH 0
#define KCDK_HASH_OFFSET_NONCE KCDK_HASH_LENGTH_OLD_HASH

#define ROTL32(v,n) (((v) << (n)) | ((v) >> (32 - (n))))

void kcdk_hash_prepare(kcdk_job_context_t *job_ctx);
void kcdk_hash_sha1(kcdk_job_context_t *job_ctx);

#endif
