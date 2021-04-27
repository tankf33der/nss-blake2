#include <stdlib.h>
#include <stdio.h>
#include "blapi.h"
#include "blapit.h"
#include "blake2b.h"

// Loup Valiant is hero
void print_vector(const uint8_t *buf, size_t size)
{
    for(size_t i = 0; i < size; i++) {
        printf("%02x", buf[i]);
    }
    printf(":\n");
}

int main(void) {
	struct Blake2bContextStr ctx;
	unsigned char d[64];
	unsigned char r[64];
	int s = 0;

	s = BLAKE2B_Hash(d, r);
	printf("init: %d\n", s);
	// s = BLAKE2B_Update(&ctx, d, 1);
	// printf("update: %d\n", s);
	// s = BLAKE2B_End(&ctx, r, NULL, 64);
	// printf("end: %d\n", s);
// 
	// print_vector(r, 64);
	// 

	return 0;
}
