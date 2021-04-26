#include <stdlib.h>
#include <stdio.h>
#include "nss-blake2.h"

// Loup Valiant is hero
void print_vector(const uint8_t *buf, size_t size)
{
    for(size_t i = 0; i < size; i++) {
        printf("%02x", buf[i]);
    }
    printf(":\n");
}


int main(void) {
	BLAKE2BContext ctx;
	unsigned char d[64];
	unsigned char r[64];
	unsigned int l;

	blake2b_Begin(&ctx, 64, d, 64);
	BLAKE2B_End(&ctx, r, &l, 64);
	print_vector(r, 64);
	

	return 0;
}
