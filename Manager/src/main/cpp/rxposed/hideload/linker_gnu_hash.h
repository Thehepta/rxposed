//
// Created by chic on 2023/12/22.
//

#pragma once

#include <stdint.h>
#include <utility>

#if defined(__arm__) || defined(__aarch64__)
#define USE_GNU_HASH_NEON 1
#else
#define USE_GNU_HASH_NEON 0
#endif



__attribute__((unused))
static std::pair<uint32_t, uint32_t> calculate_gnu_hash_simple(const char* name) {
    uint32_t h = 5381;
    const uint8_t* name_bytes = reinterpret_cast<const uint8_t*>(name);
#pragma unroll 8
    while (*name_bytes != 0) {
        h += (h << 5) + *name_bytes++; // h*33 + c = h + h * 32 + c = h + h << 5 + c
    }
    return { h, reinterpret_cast<const char*>(name_bytes) - name };
}

static inline std::pair<uint32_t, uint32_t> calculate_gnu_hash(const char* name) {
//#if USE_GNU_HASH_NEON
//    return calculate_gnu_hash_neon(name);
//#else
    return calculate_gnu_hash_simple(name);
//#endif
}
