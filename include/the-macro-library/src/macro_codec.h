// SPDX-FileCopyrightText: 2019–2026 Andy Curtis <contactandyc@gmail.com>
// SPDX-FileCopyrightText: 2024–2025 Knode.ai
// SPDX-License-Identifier: Apache-2.0
//
// Maintainer: Andy Curtis <contactandyc@gmail.com>

#ifndef _MACRO_CODEC_IMPL_H
#define _MACRO_CODEC_IMPL_H

static inline void macro_encode_u32_le(uint8_t *dst, uint32_t v) {
    dst[0] = v & 0xFF;
    dst[1] = (v >> 8) & 0xFF;
    dst[2] = (v >> 16) & 0xFF;
    dst[3] = (v >> 24) & 0xFF;
}

static inline uint32_t macro_decode_u32_le(const uint8_t *src) {
    return (uint32_t)src[0] | ((uint32_t)src[1] << 8) |
           ((uint32_t)src[2] << 16) | ((uint32_t)src[3] << 24);
}

static inline void macro_encode_u64_le(uint8_t *dst, uint64_t v) {
    for (int i = 0; i < 8; i++) dst[i] = (v >> (i * 8)) & 0xFF;
}

static inline uint64_t macro_decode_u64_le(const uint8_t *src) {
    uint64_t v = 0;
    for (int i = 0; i < 8; i++) v |= ((uint64_t)src[i]) << (i * 8);
    return v;
}

static inline int macro_encode_varint32(uint8_t *dst, uint32_t value) {
    int bytes = 0;
    while (value >= 128) {
        dst[bytes++] = (value & 127) | 128;
        value >>= 7;
    }
    dst[bytes++] = value & 127;
    return bytes;
}

static inline int macro_decode_varint32(const uint8_t *src, uint32_t limit, uint32_t *value) {
    uint32_t result = 0;
    int shift = 0;
    uint32_t bytes = 0;
    while (bytes < limit && bytes < 5) {
        uint8_t b = src[bytes++];
        result |= (uint32_t)(b & 127) << shift;
        if (!(b & 128)) {
            *value = result;
            return bytes;
        }
        shift += 7;
    }
    return 0; /* Malformed or out of bounds */
}

#endif /* _MACRO_CODEC_IMPL_H */
