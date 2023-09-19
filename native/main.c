#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../blst/bindings/blst.h"

const size_t MESSAGE_LEN = 6;

int main(int argc, char* argv[]) {
    char hex_input[300];
    unsigned char public_key[48];
    unsigned char signature[96];
    unsigned char message[6];

    if (argc != 2) {
        printf("Usage: %s <hex_string>\n", argv[0]);
        return 1;
    }
    strncpy(hex_input, argv[1], sizeof(hex_input));

    // Convert hex string to bytes.
    for (int i = 0; i < 48 * 2; i += 2) {
        sscanf(hex_input + i, "%2hhx", &public_key[i / 2]);
    }

    for (int i = 0; i < 96 * 2; i += 2) {
        sscanf(hex_input + 48 * 2 + i, "%2hhx", &signature[i / 2]);
    }

    for (int i = 0; i < MESSAGE_LEN * 2; i += 2) {
        sscanf(hex_input + 48 * 2 + 96 * 2 + i, "%2hhx", &message[i / 2]);
    }

    printf("Public Key: ");
    for (int i = 0; i < 48; i++) {
        printf("%02x", public_key[i]);
    }
    printf("\n");

    printf("Signature: ");
    for (int i = 0; i < 96; i++) {
        printf("%02x", signature[i]);
    }
    printf("\n");

    printf("Message: ");
    for (int i = 0; i < MESSAGE_LEN; i++) {
        printf("%02x", message[i]);
    }
    printf("\n");

    blst_p1_affine deserialized_pk;
    blst_p2_affine deserialized_signature;

    BLST_ERROR pk_err = blst_p1_deserialize(&deserialized_pk, public_key);
    BLST_ERROR sig_err = blst_p2_deserialize(&deserialized_signature, signature);

    if (pk_err != BLST_SUCCESS) {
        printf("Error deserializing public key.\n");
        return 1;
    }
    if (sig_err != BLST_SUCCESS) {
        printf("Error deserializing sig.\n");
        return 1;
    }

    byte DST[] = "BLS_SIG_BLS12381G2_XMD:SHA-256_SSWU_RO_POP_";
    bool hash_or_encode = true;

    BLST_ERROR verify_err = blst_core_verify_pk_in_g1(
        &deserialized_pk, 
        &deserialized_signature, 
        hash_or_encode, 
        message, 
        MESSAGE_LEN, 
        DST, 
        sizeof(DST) - 1, 
        NULL, 
        0
    );

    if (verify_err != BLST_SUCCESS) {
        printf("Failed to verify sig.\n");
        return 1;
    }
    printf("Verified sig \n");

    return 0;
}