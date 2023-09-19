#include "stylus.h"
#include "blst/bindings/blst.h"
#include <stdbool.h>

const size_t SIZE_OF_SERIALIZED_PK = 48;
const size_t SIZE_OF_SERIALIZED_SIGNATURE = 96;
const size_t MESSAGE_LEN = 6;

ArbResult user_main(uint8_t * args, size_t args_len) {
    byte *serialized_pk = args;
    byte *serialized_signature = serialized_pk + SIZE_OF_SERIALIZED_PK;
    byte *message = serialized_signature + SIZE_OF_SERIALIZED_SIGNATURE;

    blst_p1_affine deserialized_pk;
    blst_p2_affine deserialized_signature;

    BLST_ERROR pk_err = blst_p1_deserialize(&deserialized_pk, serialized_pk);
    BLST_ERROR sig_err = blst_p2_deserialize(&deserialized_signature, serialized_signature);

    if (pk_err != BLST_SUCCESS || sig_err != BLST_SUCCESS) {
        return (ArbResult) {
            .status = Failure,
            .output = args,
            .output_len = sizeof(args)
        };
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
        return (ArbResult) {
            .status = Failure,
            .output = args,
            .output_len = sizeof(args)
        };
    }

    return (ArbResult) {
        .status = Success,
        .output = args,
        .output_len = sizeof(args)
    };
}

// sets user_main as the entrypoint
ENTRYPOINT(user_main);

void* memcpy(void* dest, const void* src, size_t n) {
    unsigned char* d = dest;
    const unsigned char* s = src;
    while (n--) {
        *d++ = *s++;
    }
    return dest;
}
