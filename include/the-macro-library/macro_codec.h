// SPDX-FileCopyrightText: 2019–2026 Andy Curtis <contactandyc@gmail.com>
// SPDX-FileCopyrightText: 2024–2025 Knode.ai
// SPDX-License-Identifier: Apache-2.0
//
// Maintainer: Andy Curtis <contactandyc@gmail.com>

#ifndef _MACRO_CODEC_H
#define _MACRO_CODEC_H

#include <stdint.h>
#include <stddef.h>

/* --------------------------------------------------------------------------
 * Fixed-Width Little-Endian Encoding/Decoding
 * -------------------------------------------------------------------------- */

static inline void macro_encode_u32_le(uint8_t *dst, uint32_t v);
static inline uint32_t macro_decode_u32_le(const uint8_t *src);

static inline void macro_encode_u64_le(uint8_t *dst, uint64_t v);
static inline uint64_t macro_decode_u64_le(const uint8_t *src);

/* --------------------------------------------------------------------------
 * Variable-Length Encoding (Varints)
 * -------------------------------------------------------------------------- */

/* Returns the number of bytes written */
static inline int macro_encode_varint32(uint8_t *dst, uint32_t value);

/* Returns the number of bytes read, or 0 on bounds/format error */
static inline int macro_decode_varint32(const uint8_t *src, uint32_t limit, uint32_t *value);

#include "the-macro-library/src/macro_codec.h"

#endif /* _MACRO_CODEC_H */
