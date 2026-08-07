// SPDX-FileCopyrightText: 2019–2026 Andy Curtis <contactandyc@gmail.com>
// SPDX-FileCopyrightText: 2024–2025 Knode.ai
// SPDX-License-Identifier: Apache-2.0
//
// Maintainer: Andy Curtis <contactandyc@gmail.com>

#ifndef MACRO_ID_ARRAY_H
#define MACRO_ID_ARRAY_H

#include <stdint.h>
#include <string.h>

/*
 * Dynamic Radix Array / Page Table
 * PAGE_BITS: Block size (e.g., 8 = 256 items per block)
 * CTX_TYPE: The type of your allocator context (e.g., aml_pool_t *)
 * ZALLOC_FUNC: The function that returns zeroed memory (e.g., aml_pool_zalloc)
 */
#define macro_id_array(NAME, TYPE, PAGE_BITS, CTX_TYPE, ZALLOC_FUNC) \
typedef struct { \
    void *root; \
    uint32_t count; \
    uint8_t depth; \
    CTX_TYPE ctx; \
} NAME##_t; \
\
static inline void NAME##_init(NAME##_t *arr, CTX_TYPE ctx) { \
    arr->ctx = ctx; \
    arr->count = 0; \
    arr->depth = 1; \
    arr->root = ZALLOC_FUNC(ctx, (1 << (PAGE_BITS)) * sizeof(TYPE)); \
} \
\
static inline void NAME##_push(NAME##_t *arr, TYPE value) { \
    uint32_t id = arr->count; \
    uint64_t capacity = 1; \
    for (uint8_t i = 0; i < arr->depth; i++) capacity <<= (PAGE_BITS); \
    \
    if (id >= capacity) { \
        void **new_root = (void **)ZALLOC_FUNC(arr->ctx, (1 << (PAGE_BITS)) * sizeof(void *)); \
        new_root[0] = arr->root; \
        arr->root = new_root; \
        arr->depth++; \
    } \
    \
    void **curr = (void **)arr->root; \
    for (int i = arr->depth - 1; i > 0; i--) { \
        uint32_t shift = i * (PAGE_BITS); \
        uint32_t idx = (id >> shift) & ((1 << (PAGE_BITS)) - 1); \
        if (!curr[idx]) { \
            if (i == 1) { \
                curr[idx] = ZALLOC_FUNC(arr->ctx, (1 << (PAGE_BITS)) * sizeof(TYPE)); \
            } else { \
                curr[idx] = ZALLOC_FUNC(arr->ctx, (1 << (PAGE_BITS)) * sizeof(void *)); \
            } \
        } \
        curr = (void **)curr[idx]; \
    } \
    \
    TYPE *leaf = (TYPE *)curr; \
    uint32_t leaf_idx = id & ((1 << (PAGE_BITS)) - 1); \
    leaf[leaf_idx] = value; \
    arr->count++; \
} \
\
static inline TYPE NAME##_get(NAME##_t *arr, uint32_t id) { \
    if (id >= arr->count) { \
        TYPE empty; \
        memset(&empty, 0, sizeof(TYPE)); \
        return empty; \
    } \
    void **curr = (void **)arr->root; \
    for (int i = arr->depth - 1; i > 0; i--) { \
        uint32_t shift = i * (PAGE_BITS); \
        uint32_t idx = (id >> shift) & ((1 << (PAGE_BITS)) - 1); \
        curr = (void **)curr[idx]; \
        if (!curr) { \
            TYPE empty; memset(&empty, 0, sizeof(TYPE)); return empty; \
        } \
    } \
    TYPE *leaf = (TYPE *)curr; \
    uint32_t leaf_idx = id & ((1 << (PAGE_BITS)) - 1); \
    return leaf[leaf_idx]; \
}

#endif // MACRO_ID_ARRAY_H
