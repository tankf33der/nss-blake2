#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "nss.h"
#include "pk11pub.h"
#include "hasht.h"

// Loup Valiant is hero
void print_vector(const uint8_t *buf, size_t size)
{
    for(size_t i = 0; i < size; i++) {
        printf("%02x", buf[i]);
    }
    printf(":\n");
}


int main(void) {
	unsigned char d[32];
	unsigned char r[32];
	unsigned int len;
 	PK11Context *ctx;
 	SECStatus status;

	HASHContext* HASH_Create(HASH_HashType type);


  	NSS_NoDB_Init(NULL);

 	memset(d, 0xbc, sizeof d);
 	
 	ctx = PK11_CreateDigestContext(SEC_OID_SHA256);
 	PR_ASSERT(ctx != NULL);

 	status = PK11_DigestBegin(ctx);
 	PR_ASSERT(status == SECSuccess);

 	PK11_DigestFinal(ctx, r, &len, sizeof r);
	print_vector(r, sizeof r); 	
	PK11_DestroyContext(ctx, PR_TRUE);

	NSS_Shutdown();
	return 0;
}
