#include <stdio.h>
#include <string.h>
#include "NuMicro.h"
#include "boot_system.h"

/* SHA-1 hash */
char sha_msg[] = "608079423f12421de616b7493ebe551cf4d65b92";

/* public key 1 */
char Qx[] = "07008ea40b08dbe76432096e80a2494c94982d2d5bcf98e6";

/* public key 2 */
char Qy[] = "76fab681d00b414ea636ba215de26d98c41bd7f2e4d65477";

/* Expected answer: R of (R,S) digital signature */
char R[] = "6994d962bdd0d793ffddf855ec5bf2f91a9698b46258a63e";

/* Expected answer: S of (R,S) digital signature */
char S[] = "02ba6465a234903744ab02bc8521405b73cf5fc00e1a9f41";

void CRYPTO_IRQHandler()
{
    ECC_Complete(CRPT);
}

void dump_buff_hex(uint8_t *pucBuff, int nBytes)
{
    int nIdx, i;

    nIdx = 0;
    while (nBytes > 0) {
        printf("0x%04X  ", nIdx);
        for (i = 0; i < 16; i++)
            printf("%02x ", pucBuff[nIdx + i]);
        printf("  ");
        for (i = 0; i < 16; i++) {
            if ((pucBuff[nIdx + i] >= 0x20) && (pucBuff[nIdx + i] < 127))
                printf("%c", pucBuff[nIdx + i]);
            else
                printf(".");
            nBytes--;
        }
        nIdx += 16;
        printf("\n");
    }
    printf("\n");
}

int main(void)
{
    system_init();

    NVIC_EnableIRQ(CRPT_IRQn);
    ECC_ENABLE_INT(CRPT);

    printf("+---------------------------------------------+\n");
    printf("|   Crypto ECC Public Key Verification Demo    |\n");
    printf("+---------------------------------------------+\n");

    if (ECC_VerifySignature(CRPT, CURVE_P_192, sha_msg, Qx, Qy, R, S) < 0) {
        printf("ECC signature verification failed!!\n");
        while (1)
            ;
    }

    printf("ECC digital signature verification OK.\n");

    while (1)
        ;
}
