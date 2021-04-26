#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#define min(a,b) (((a) < (b)) ? (a) : (b))

#include "nss-blake2.h"

static const uint64_t nss_iv[8] = {
    0x6a09e667f3bcc908ULL, 0xbb67ae8584caa73bULL, 0x3c6ef372fe94f82bULL,
    0xa54ff53a5f1d36f1ULL, 0x510e527fade682d1ULL, 0x9b05688c2b3e6c1fULL,
    0x1f83d9abfb41bd6bULL, 0x5be0cd19137e2179ULL
};

/**
 * This contains the table of permutations for blake2b compression function.
 */
static const uint8_t nss_sigma[12][16] = {
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
    { 14, 10, 4, 8, 9, 15, 13, 6, 1, 12, 0, 2, 11, 7, 5, 3 },
    { 11, 8, 12, 0, 5, 2, 15, 13, 10, 14, 3, 6, 7, 1, 9, 4 },
    { 7, 9, 3, 1, 13, 12, 11, 14, 2, 6, 5, 10, 4, 0, 15, 8 },
    { 9, 0, 5, 7, 2, 4, 10, 15, 14, 1, 11, 12, 6, 8, 3, 13 },
    { 2, 12, 6, 10, 0, 11, 8, 3, 4, 13, 7, 5, 15, 14, 1, 9 },
    { 12, 5, 1, 15, 14, 13, 4, 10, 0, 7, 6, 3, 9, 2, 8, 11 },
    { 13, 11, 7, 14, 12, 1, 3, 9, 5, 0, 15, 4, 8, 6, 2, 10 },
    { 6, 15, 14, 9, 11, 3, 0, 8, 12, 2, 13, 7, 1, 4, 10, 5 },
    { 10, 2, 8, 4, 7, 6, 1, 5, 15, 11, 9, 14, 3, 12, 13, 0 },
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
    { 14, 10, 4, 8, 9, 15, 13, 6, 1, 12, 0, 2, 11, 7, 5, 3 }
};


/**
 * This function increments the blake2b ctx counter.
 */
void
blake2b_IncrementCounter(BLAKE2BContext* ctx, const uint64_t inc)
{
    ctx->t[0] += inc;
    ctx->t[1] += ctx->t[0] < inc;
}

static uint64_t rotr64(uint64_t x, uint64_t n) { return (x >> n) ^ (x << (64 - n)); }
static uint32_t rotl32(uint32_t x, uint32_t n) { return (x << n) ^ (x >> (32 - n)); }

/**
 * This macro implements the blake2b mixing function which mixes two 8-byte
 * words from the message into the hash.
 */
#define NSSG(a, b, c, d, x, y) \
    a += b + x;             \
    d = rotr64(d ^ a, 32);  \
    c += d;                 \
    b = rotr64(b ^ c, 24);  \
    a += b + y;             \
    d = rotr64(d ^ a, 16);  \
    c += d;                 \
    b = rotr64(b ^ c, 63)

#define NSSROUND(i)                                                   \
    NSSG(v[0], v[4], v[8], v[12], m[nss_sigma[i][0]], m[nss_sigma[i][1]]);    \
    NSSG(v[1], v[5], v[9], v[13], m[nss_sigma[i][2]], m[nss_sigma[i][3]]);    \
    NSSG(v[2], v[6], v[10], v[14], m[nss_sigma[i][4]], m[nss_sigma[i][5]]);   \
    NSSG(v[3], v[7], v[11], v[15], m[nss_sigma[i][6]], m[nss_sigma[i][7]]);   \
    NSSG(v[0], v[5], v[10], v[15], m[nss_sigma[i][8]], m[nss_sigma[i][9]]);   \
    NSSG(v[1], v[6], v[11], v[12], m[nss_sigma[i][10]], m[nss_sigma[i][11]]); \
    NSSG(v[2], v[7], v[8], v[13], m[nss_sigma[i][12]], m[nss_sigma[i][13]]);  \
    NSSG(v[3], v[4], v[9], v[14], m[nss_sigma[i][14]], m[nss_sigma[i][15]])

void
blake2b_Compress(BLAKE2BContext* ctx, const uint8_t* block)
{
    size_t i;
    uint64_t v[16], m[16];

    memcpy(m, block, BLAKE2B_BLOCK_LENGTH);

    memcpy(v, ctx->h, 8 * 8);
    memcpy(v + 8, nss_iv, 8 * 8);

    v[12] ^= ctx->t[0];
    v[13] ^= ctx->t[1];
    v[14] ^= ctx->f;

    NSSROUND(0);
    NSSROUND(1);
    NSSROUND(2);
    NSSROUND(3);
    NSSROUND(4);
    NSSROUND(5);
    NSSROUND(6);
    NSSROUND(7);
    NSSROUND(8);
    NSSROUND(9);
    NSSROUND(10);
    NSSROUND(11);

    for (i = 0; i < 8; i++) {
        ctx->h[i] ^= v[i] ^ v[i + 8];
    }
}

int
blake2b_Begin(BLAKE2BContext* ctx, uint8_t outlen, const uint8_t* key,
              size_t keylen)
{
    if (!ctx) {
        goto failure_noclean;
    }
    if (outlen == 0 || outlen > 64) {
        goto failure;
    }
    if (key && keylen > 64) {
        goto failure;
    }
    /* Note: key can be null if it's unkeyed. */
    if ((key == NULL && keylen > 0) || keylen > 64 ||
        (key != NULL && keylen == 0)) {
        goto failure;
    }

    /* Mix key size(keylen) and desired hash length(outlen) into h0 */
    uint64_t param = outlen ^ (keylen << 8) ^ (1 << 16) ^ (1 << 24);
    memcpy(ctx->h, nss_iv, 8 * 8);
    ctx->h[0] ^= param;
    ctx->outlen = outlen;

    /* This updates the context for only the keyed version */
    if (keylen > 0 && keylen <= 64 && key) {
        uint8_t block[BLAKE2B_BLOCK_LENGTH] = { 0 };
        memcpy(block, key, keylen);
        BLAKE2B_Update(ctx, block, BLAKE2B_BLOCK_LENGTH);
        memset(block, 0, BLAKE2B_BLOCK_LENGTH);
    }

    return 1;

failure:
    memset(ctx, 0, sizeof(*ctx));
failure_noclean:
    return 0;
}

static void
blake2b_IncrementCompress(BLAKE2BContext* ctx, size_t blockLength,
                          const unsigned char* input)
{
    blake2b_IncrementCounter(ctx, blockLength);
    blake2b_Compress(ctx, input);
}

int
BLAKE2B_Update(BLAKE2BContext* ctx, const unsigned char* in,
               unsigned int inlen)
{
    /* Nothing to do if there's nothing. */
    if (inlen == 0) {
        return 1;
    }

    if (!ctx || !in) {
        return 10;
    }

    /* Is this a reused context? */
    if (ctx->f) {
        return 20;
    }

    size_t left = ctx->buflen;
    assert(left <= BLAKE2B_BLOCK_LENGTH);
    size_t fill = BLAKE2B_BLOCK_LENGTH - left;

    if (inlen > fill) {
        if (ctx->buflen) {
            /* There's some remaining data in ctx->buf that we have to prepend
             * to in. */
            memcpy(ctx->buf + left, in, fill);
            ctx->buflen = 0;
            blake2b_IncrementCompress(ctx, BLAKE2B_BLOCK_LENGTH, ctx->buf);
            in += fill;
            inlen -= fill;
        }
        while (inlen > BLAKE2B_BLOCK_LENGTH) {
            blake2b_IncrementCompress(ctx, BLAKE2B_BLOCK_LENGTH, in);
            in += BLAKE2B_BLOCK_LENGTH;
            inlen -= BLAKE2B_BLOCK_LENGTH;
        }
    }

    /* Store the remaining data from in in ctx->buf to process later.
     * Note that ctx->buflen can be BLAKE2B_BLOCK_LENGTH. We can't process that
     * here because we have to update ctx->f before compressing the last block.
     */
    assert(inlen <= BLAKE2B_BLOCK_LENGTH);
    memcpy(ctx->buf + ctx->buflen, in, inlen);
    ctx->buflen += inlen;

    return 1;
}

int
BLAKE2B_End(BLAKE2BContext* ctx, unsigned char* out,
            unsigned int* digestLen, size_t maxDigestLen)
{
    size_t i;
    unsigned int outlen = min(BLAKE2B512_LENGTH, maxDigestLen);

    /* Argument checks */
    if (!ctx || !out) {
        return 10;
    }

    /* Sanity check against outlen in context. */
    if (ctx->outlen < outlen) {
        return 20;
    }

    /* Is this a reused context? */
    if (ctx->f != 0) {
        return 30;
    }

    /* Process the remaining data from ctx->buf (padded with 0). */
    blake2b_IncrementCounter(ctx, ctx->buflen);
    /* BLAKE2B_BLOCK_LENGTH - ctx->buflen can be 0. */
    memset(ctx->buf + ctx->buflen, 0, BLAKE2B_BLOCK_LENGTH - ctx->buflen);
    ctx->f = UINT64_MAX;
    blake2b_Compress(ctx, ctx->buf);

    /* Write out the blake2b context(ctx). */
    for (i = 0; i < outlen; ++i) {
        out[i] = ctx->h[i / 8] >> ((i % 8) * 8);
    }

    if (digestLen) {
        *digestLen = outlen;
    }

    return 1;
}
