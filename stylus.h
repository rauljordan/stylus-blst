// Copyright 2022-2023, Offchain Labs, Inc.
// For licensing, see https://github.com/stylus-sdk-c/blob/stylus/licenses/COPYRIGHT.md
#ifndef STYLUS_STYLUS_H
#define STYLUS_STYLUS_H

#include <stddef.h>
#include <stdint.h>

#define VM_HOOK(name) extern __attribute__((import_module("vm_hooks"), import_name(#name)))

VM_HOOK(read_args) void read_args(const uint8_t * data);
VM_HOOK(write_result) void write_result(const uint8_t * data, size_t len);
VM_HOOK(memory_grow) void memory_grow(const uint16_t pages);

typedef enum ArbStatus {
    Success = 0,
    Failure,
} ArbStatus;

typedef struct ArbResult {
    const ArbStatus status;
    const uint8_t * output;
    const size_t output_len;
} ArbResult;

#define ENTRYPOINT(user_main)                                           \
    /* Force the compiler to import these symbols                    */ \
    /* Note: calling these functions will unproductively consume gas */ \
    __attribute__((export_name("mark_used")))                           \
    void mark_used() {                                                  \
        memory_grow(0);                                                 \
    }                                                                   \
                                                                        \
    __attribute__((export_name("user_entrypoint")))                     \
    int user_entrypoint(size_t args_len) {                              \
        uint8_t args[args_len];                                         \
        read_args(args);                                                \
        const ArbResult result = user_main(args, args_len);             \
        write_result(result.output, result.output_len);                 \
        return result.status;                                           \
    }
#endif
