#define BLAKE2B_BLOCK_LENGTH 128 
#define BLAKE2B512_LENGTH 64 


struct Blake2bContextStr {
    uint64_t h[8];                     /* chained state */
    uint64_t t[2];                     /* total number of bytes */
    uint64_t f;                        /* last block flag */
    uint8_t buf[BLAKE2B_BLOCK_LENGTH]; /* input buffer */
    size_t buflen;                     /* size of remaining bytes in buf */
    size_t outlen;                     /* digest size */
};
typedef struct Blake2bContextStr BLAKE2BContext;

static int
blake2b_Begin(BLAKE2BContext* ctx, uint8_t outlen, const uint8_t* key,
              size_t keylen);
static int
BLAKE2B_Update(BLAKE2BContext* ctx, const unsigned char* in,
               unsigned int inlen);
static int
BLAKE2B_End(BLAKE2BContext* ctx, unsigned char* out,
            unsigned int* digestLen, size_t maxDigestLen);
