/**
 * @file test_hash.c
 * @author cy023
 * @date 2023.03.13
 * @brief
 *
 */

#include <stdio.h>
#include <string.h>
#include "NuMicro.h"
#include "boot_system.h"

#include "mbedtls/sha256.h"

int main()
{
    system_init();

    printf("System Boot.\n");
    printf("[Test]: SHA-256 ...\n");

    const char *text = "Hello. Here is hash function test.";
    unsigned char hash[32];
    unsigned char real[32] = {0x5a, 0x20, 0x14, 0x72, 0x26, 0xcf, 0x18, 0x48,
                              0xe9, 0x1b, 0xab, 0xcc, 0xa3, 0x94, 0x4e, 0x1d,
                              0x6b, 0xad, 0xda, 0x52, 0xc2, 0xbb, 0xa8, 0xb6,
                              0x44, 0xc3, 0x4f, 0x13, 0xf8, 0x3c, 0xe7, 0x1f};

    mbedtls_sha256((const unsigned char *) text, strlen(text), hash, 0);

    printf("Text: %s\n", text);
    printf("SHA256 hash of Text: ");
    for (int i = 0; i < 32; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n");

    for (int i = 0; i < 32; i++) {
        if (hash[i] != real[i])
            printf("Verify Failed ...\n");
    }
    printf("Verify OK !\n");

    return 0;
}
