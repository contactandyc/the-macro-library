// SPDX-FileCopyrightText: 2019–2026 Andy Curtis <contactandyc@gmail.com>
// SPDX-FileCopyrightText: 2024–2025 Knode.ai
// SPDX-License-Identifier: Apache-2.0
//
// Maintainer: Andy Curtis <contactandyc@gmail.com>

#ifndef _macro_skiplist_H
#define _macro_skiplist_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>

#include "the-macro-library/macro_cmp.h"
#include "the-macro-library/macro_parent_object.h"

/* Cross-compatible atomics for C11/C23 and C++17 */
#ifndef MACRO_ATOMIC_PTR
#ifdef __cplusplus
#include <atomic>
#define MACRO_ATOMIC_PTR(type) std::atomic<type*>
#define MACRO_ATOMIC_LOAD(ptr, order) std::atomic_load_explicit(ptr, std::memory_order_##order)
#define MACRO_ATOMIC_STORE(ptr, val, order) std::atomic_store_explicit(ptr, val, std::memory_order_##order)
#define MACRO_ATOMIC_INIT(ptr, val) std::atomic_init(ptr, val)
#define MACRO_ATOMIC_FENCE(order) std::atomic_thread_fence(std::memory_order_##order)
#else
#include <stdatomic.h>
#define MACRO_ATOMIC_PTR(type) _Atomic(type*)
#define MACRO_ATOMIC_LOAD(ptr, order) atomic_load_explicit(ptr, memory_order_##order)
#define MACRO_ATOMIC_STORE(ptr, val, order) atomic_store_explicit(ptr, val, memory_order_##order)
#define MACRO_ATOMIC_INIT(ptr, val) atomic_init(ptr, val)
#define MACRO_ATOMIC_FENCE(order) atomic_thread_fence(memory_order_##order)
#endif
#endif

typedef struct macro_skiplist_s {
    uint8_t height;
    MACRO_ATOMIC_PTR(struct macro_skiplist_s) forward[];
} macro_skiplist_t;

static inline void macro_skiplist_init_head(macro_skiplist_t *head, uint8_t max_height) {
    head->height = max_height;
    for(int i = 0; i < max_height; i++) {
        MACRO_ATOMIC_INIT(&head->forward[i], NULL);
    }
}

static inline uint8_t macro_skiplist_random_height(uint8_t max_height) {
    uint8_t h = 1;
    while (h < max_height && (rand() % 2) == 0) {
        h++;
    }
    return h;
}

#include "the-macro-library/src/macro_skiplist_code.h"

/* --------------------------------------------------------------------------
 * Find API (Standard, Lower Bound, Upper Bound, Floor, Ceiling)
 * -------------------------------------------------------------------------- */

#define _macro_skiplist_h(name, style, type) \
    type *name(const macro_skiplist_t *head, macro_cmp_signature(const type *key, style, type))

#define _macro_skiplist_with_field(name, field, find_style, style, type, cmp) \
    _macro_skiplist_h(name, style, type) { \
        _macro_skiplist_##find_style##_code_with_field(head, field, style, type, cmp, key); \
    }

#define _macro_skiplist(name, find_style, style, type, cmp) \
    _macro_skiplist_h(name, style, type) { \
        _macro_skiplist_##find_style##_code(head, style, type, cmp, key); \
    }

/* KV Variants */
#define _macro_skiplist_kv_h(name, style, key_type, value_type) \
    value_type *name(const macro_skiplist_t *head, macro_cmp_kv_signature(const key_type *key, style, key_type, value_type))

#define _macro_skiplist_kv_with_field(name, field, find_style, style, key_type, value_type, cmp) \
    _macro_skiplist_kv_h(name, style, key_type, value_type) { \
        _macro_skiplist_kv_##find_style##_code_with_field(head, field, style, key_type, value_type, cmp, key); \
    }

#define _macro_skiplist_kv(name, find_style, style, key_type, value_type, cmp) \
    _macro_skiplist_kv_h(name, style, key_type, value_type) { \
        _macro_skiplist_kv_##find_style##_code(head, style, key_type, value_type, cmp, key); \
    }

/* --------------------------------------------------------------------------
 * Insert API (Standard and Multi)
 * -------------------------------------------------------------------------- */

#define _macro_skiplist_insert_h(name, style, type) \
    bool name(macro_skiplist_t *head, macro_cmp_signature(type *node, style, type))

#define _macro_skiplist_insert_with_field(name, field, style, type, cmp) \
    _macro_skiplist_insert_h(name, style, type) { \
        _macro_skiplist_insert_code_with_field(head, field, style, type, cmp, node); \
    }

#define _macro_skiplist_insert(name, style, type, cmp) \
    _macro_skiplist_insert_h(name, style, type) { \
        _macro_skiplist_insert_code(head, style, type, cmp, node); \
    }

#define _macro_multi_skiplist_insert_with_field(name, field, style, type, cmp) \
    _macro_skiplist_insert_h(name, style, type) { \
        _macro_multi_skiplist_insert_code_with_field(head, field, style, type, cmp, node); \
    }

#define _macro_multi_skiplist_insert(name, style, type, cmp) \
    _macro_skiplist_insert_h(name, style, type) { \
        _macro_multi_skiplist_insert_code(head, style, type, cmp, node); \
    }

/* --------------------------------------------------------------------------
 * Default API Macros (Using cmp_no_arg)
 * -------------------------------------------------------------------------- */
#define macro_skiplist_default() cmp_no_arg

#define macro_skiplist_find_with_field(name, field, type, cmp) _macro_skiplist_with_field(name, field, find, macro_skiplist_default(), type, cmp)
#define macro_skiplist_find(name, type, cmp) _macro_skiplist(name, find, macro_skiplist_default(), type, cmp)

#define macro_skiplist_insert_with_field(name, field, type, cmp) _macro_skiplist_insert_with_field(name, field, macro_skiplist_default(), type, cmp)
#define macro_skiplist_insert(name, type, cmp) _macro_skiplist_insert(name, macro_skiplist_default(), type, cmp)

#define macro_multi_skiplist_insert_with_field(name, field, type, cmp) _macro_multi_skiplist_insert_with_field(name, field, macro_skiplist_default(), type, cmp)
#define macro_multi_skiplist_insert(name, type, cmp) _macro_multi_skiplist_insert(name, macro_skiplist_default(), type, cmp)

/* ==========================================================================
 * INTERNAL GENERATOR ENGINE (Linkage Parameterized)
 * ========================================================================== */
/* Standard Skiplist */
#define __macro_skiplist_api_h(MOD, prefix, type) \
    MOD _macro_skiplist_h(prefix##_find, macro_skiplist_default(), type); \
    MOD _macro_skiplist_h(prefix##_lower_bound, macro_skiplist_default(), type); \
    MOD _macro_skiplist_h(prefix##_upper_bound, macro_skiplist_default(), type); \
    MOD _macro_skiplist_h(prefix##_floor, macro_skiplist_default(), type); \
    MOD _macro_skiplist_h(prefix##_ceiling, macro_skiplist_default(), type); \
    MOD _macro_skiplist_insert_h(prefix##_insert, macro_skiplist_default(), type)

#define __macro_skiplist_api_h_with_field(MOD, prefix, field, type) \
    MOD _macro_skiplist_h(prefix##_find, macro_skiplist_default(), type); \
    MOD _macro_skiplist_h(prefix##_lower_bound, macro_skiplist_default(), type); \
    MOD _macro_skiplist_h(prefix##_upper_bound, macro_skiplist_default(), type); \
    MOD _macro_skiplist_h(prefix##_floor, macro_skiplist_default(), type); \
    MOD _macro_skiplist_h(prefix##_ceiling, macro_skiplist_default(), type); \
    MOD _macro_skiplist_insert_h(prefix##_insert, macro_skiplist_default(), type)

#define __macro_skiplist_api(MOD, prefix, type, cmp) \
    MOD _macro_skiplist(prefix##_find, find, macro_skiplist_default(), type, cmp) \
    MOD _macro_skiplist(prefix##_lower_bound, lower_bound, macro_skiplist_default(), type, cmp) \
    MOD _macro_skiplist(prefix##_upper_bound, upper_bound, macro_skiplist_default(), type, cmp) \
    MOD _macro_skiplist(prefix##_floor, floor, macro_skiplist_default(), type, cmp) \
    MOD _macro_skiplist(prefix##_ceiling, ceiling, macro_skiplist_default(), type, cmp) \
    MOD _macro_skiplist_insert(prefix##_insert, macro_skiplist_default(), type, cmp)

#define __macro_skiplist_api_with_field(MOD, prefix, field, type, cmp) \
    MOD _macro_skiplist_with_field(prefix##_find, field, find, macro_skiplist_default(), type, cmp) \
    MOD _macro_skiplist_with_field(prefix##_lower_bound, field, lower_bound, macro_skiplist_default(), type, cmp) \
    MOD _macro_skiplist_with_field(prefix##_upper_bound, field, upper_bound, macro_skiplist_default(), type, cmp) \
    MOD _macro_skiplist_with_field(prefix##_floor, field, floor, macro_skiplist_default(), type, cmp) \
    MOD _macro_skiplist_with_field(prefix##_ceiling, field, ceiling, macro_skiplist_default(), type, cmp) \
    MOD _macro_skiplist_insert_with_field(prefix##_insert, field, macro_skiplist_default(), type, cmp)

/* KV Skiplist */
#define __macro_skiplist_kv_api_h(MOD, prefix, key_type, value_type) \
    MOD _macro_skiplist_kv_h(prefix##_find, macro_skiplist_default(), key_type, value_type); \
    MOD _macro_skiplist_kv_h(prefix##_lower_bound, macro_skiplist_default(), key_type, value_type); \
    MOD _macro_skiplist_kv_h(prefix##_upper_bound, macro_skiplist_default(), key_type, value_type); \
    MOD _macro_skiplist_kv_h(prefix##_floor, macro_skiplist_default(), key_type, value_type); \
    MOD _macro_skiplist_kv_h(prefix##_ceiling, macro_skiplist_default(), key_type, value_type); \
    MOD _macro_skiplist_insert_h(prefix##_insert, macro_skiplist_default(), value_type)

#define __macro_skiplist_kv_api_h_with_field(MOD, prefix, field, key_type, value_type) \
    MOD _macro_skiplist_kv_h(prefix##_find, macro_skiplist_default(), key_type, value_type); \
    MOD _macro_skiplist_kv_h(prefix##_lower_bound, macro_skiplist_default(), key_type, value_type); \
    MOD _macro_skiplist_kv_h(prefix##_upper_bound, macro_skiplist_default(), key_type, value_type); \
    MOD _macro_skiplist_kv_h(prefix##_floor, macro_skiplist_default(), key_type, value_type); \
    MOD _macro_skiplist_kv_h(prefix##_ceiling, macro_skiplist_default(), key_type, value_type); \
    MOD _macro_skiplist_insert_h(prefix##_insert, macro_skiplist_default(), value_type)

#define __macro_skiplist_kv_api(MOD, prefix, key_type, value_type, cmp) \
    MOD _macro_skiplist_kv(prefix##_find, find, macro_skiplist_default(), key_type, value_type, cmp) \
    MOD _macro_skiplist_kv(prefix##_lower_bound, lower_bound, macro_skiplist_default(), key_type, value_type, cmp) \
    MOD _macro_skiplist_kv(prefix##_upper_bound, upper_bound, macro_skiplist_default(), key_type, value_type, cmp) \
    MOD _macro_skiplist_kv(prefix##_floor, floor, macro_skiplist_default(), key_type, value_type, cmp) \
    MOD _macro_skiplist_kv(prefix##_ceiling, ceiling, macro_skiplist_default(), key_type, value_type, cmp) \
    MOD _macro_skiplist_insert(prefix##_insert, macro_skiplist_default(), value_type, cmp)

#define __macro_skiplist_kv_api_with_field(MOD, prefix, field, key_type, value_type, cmp) \
    MOD _macro_skiplist_kv_with_field(prefix##_find, field, find, macro_skiplist_default(), key_type, value_type, cmp) \
    MOD _macro_skiplist_kv_with_field(prefix##_lower_bound, field, lower_bound, macro_skiplist_default(), key_type, value_type, cmp) \
    MOD _macro_skiplist_kv_with_field(prefix##_upper_bound, field, upper_bound, macro_skiplist_default(), key_type, value_type, cmp) \
    MOD _macro_skiplist_kv_with_field(prefix##_floor, field, floor, macro_skiplist_default(), key_type, value_type, cmp) \
    MOD _macro_skiplist_kv_with_field(prefix##_ceiling, field, ceiling, macro_skiplist_default(), key_type, value_type, cmp) \
    MOD _macro_skiplist_insert_with_field(prefix##_insert, field, macro_skiplist_default(), value_type, cmp)

/* Multi Skiplist */
#define __macro_multi_skiplist_api_h(MOD, prefix, type) \
    MOD _macro_skiplist_h(prefix##_lower_bound, macro_skiplist_default(), type); \
    MOD type * prefix##_find(const macro_skiplist_t *head, const type *key); \
    MOD _macro_skiplist_h(prefix##_upper_bound, macro_skiplist_default(), type); \
    MOD _macro_skiplist_h(prefix##_floor, macro_skiplist_default(), type); \
    MOD _macro_skiplist_h(prefix##_ceiling, macro_skiplist_default(), type); \
    MOD _macro_skiplist_insert_h(prefix##_insert, macro_skiplist_default(), type)

#define __macro_multi_skiplist_api_h_with_field(MOD, prefix, field, type) \
    MOD _macro_skiplist_h(prefix##_lower_bound, macro_skiplist_default(), type); \
    MOD type * prefix##_find(const macro_skiplist_t *head, const type *key); \
    MOD _macro_skiplist_h(prefix##_upper_bound, macro_skiplist_default(), type); \
    MOD _macro_skiplist_h(prefix##_floor, macro_skiplist_default(), type); \
    MOD _macro_skiplist_h(prefix##_ceiling, macro_skiplist_default(), type); \
    MOD _macro_skiplist_insert_h(prefix##_insert, macro_skiplist_default(), type)

#define __macro_multi_skiplist_api(MOD, prefix, type, cmp) \
    MOD _macro_skiplist(prefix##_lower_bound, lower_bound, macro_skiplist_default(), type, cmp) \
    MOD type * prefix##_find(const macro_skiplist_t *head, const type *key) { \
        type *node = prefix##_lower_bound(head, key); \
        if (node && macro_equal(macro_skiplist_default(), type, cmp, key, node)) return node; \
        return NULL; \
    } \
    MOD _macro_skiplist(prefix##_upper_bound, upper_bound, macro_skiplist_default(), type, cmp) \
    MOD _macro_skiplist(prefix##_floor, floor, macro_skiplist_default(), type, cmp) \
    MOD _macro_skiplist(prefix##_ceiling, ceiling, macro_skiplist_default(), type, cmp) \
    MOD _macro_multi_skiplist_insert(prefix##_insert, macro_skiplist_default(), type, cmp)

#define __macro_multi_skiplist_api_with_field(MOD, prefix, field, type, cmp) \
    MOD _macro_skiplist_with_field(prefix##_lower_bound, field, lower_bound, macro_skiplist_default(), type, cmp) \
    MOD type * prefix##_find(const macro_skiplist_t *head, const type *key) { \
        type *node = prefix##_lower_bound(head, key); \
        if (node && macro_equal(macro_skiplist_default(), type, cmp, key, node)) return node; \
        return NULL; \
    } \
    MOD _macro_skiplist_with_field(prefix##_upper_bound, field, upper_bound, macro_skiplist_default(), type, cmp) \
    MOD _macro_skiplist_with_field(prefix##_floor, field, floor, macro_skiplist_default(), type, cmp) \
    MOD _macro_skiplist_with_field(prefix##_ceiling, field, ceiling, macro_skiplist_default(), type, cmp) \
    MOD _macro_multi_skiplist_insert_with_field(prefix##_insert, field, macro_skiplist_default(), type, cmp)

/* KV Multi Skiplist */
#define __macro_multi_skiplist_kv_api_h(MOD, prefix, key_type, value_type) \
    MOD _macro_skiplist_kv_h(prefix##_lower_bound, macro_skiplist_default(), key_type, value_type); \
    MOD value_type * prefix##_find(const macro_skiplist_t *head, const key_type *key); \
    MOD _macro_skiplist_kv_h(prefix##_upper_bound, macro_skiplist_default(), key_type, value_type); \
    MOD _macro_skiplist_kv_h(prefix##_floor, macro_skiplist_default(), key_type, value_type); \
    MOD _macro_skiplist_kv_h(prefix##_ceiling, macro_skiplist_default(), key_type, value_type); \
    MOD _macro_skiplist_insert_h(prefix##_insert, macro_skiplist_default(), value_type)

#define __macro_multi_skiplist_kv_api_h_with_field(MOD, prefix, field, key_type, value_type) \
    MOD _macro_skiplist_kv_h(prefix##_lower_bound, macro_skiplist_default(), key_type, value_type); \
    MOD value_type * prefix##_find(const macro_skiplist_t *head, const key_type *key); \
    MOD _macro_skiplist_kv_h(prefix##_upper_bound, macro_skiplist_default(), key_type, value_type); \
    MOD _macro_skiplist_kv_h(prefix##_floor, macro_skiplist_default(), key_type, value_type); \
    MOD _macro_skiplist_kv_h(prefix##_ceiling, macro_skiplist_default(), key_type, value_type); \
    MOD _macro_skiplist_insert_h(prefix##_insert, macro_skiplist_default(), value_type)

#define __macro_multi_skiplist_kv_api(MOD, prefix, key_type, value_type, cmp) \
    MOD _macro_skiplist_kv(prefix##_lower_bound, lower_bound, macro_skiplist_default(), key_type, value_type, cmp) \
    MOD value_type * prefix##_find(const macro_skiplist_t *head, const key_type *key) { \
        value_type *node = prefix##_lower_bound(head, key); \
        if (node && macro_equal_kv(macro_skiplist_default(), key_type, value_type, cmp, key, node)) return node; \
        return NULL; \
    } \
    MOD _macro_skiplist_kv(prefix##_upper_bound, upper_bound, macro_skiplist_default(), key_type, value_type, cmp) \
    MOD _macro_skiplist_kv(prefix##_floor, floor, macro_skiplist_default(), key_type, value_type, cmp) \
    MOD _macro_skiplist_kv(prefix##_ceiling, ceiling, macro_skiplist_default(), key_type, value_type, cmp) \
    MOD _macro_multi_skiplist_insert(prefix##_insert, macro_skiplist_default(), value_type, cmp)

#define __macro_multi_skiplist_kv_api_with_field(MOD, prefix, field, key_type, value_type, cmp) \
    MOD _macro_skiplist_kv_with_field(prefix##_lower_bound, field, lower_bound, macro_skiplist_default(), key_type, value_type, cmp) \
    MOD value_type * prefix##_find(const macro_skiplist_t *head, const key_type *key) { \
        value_type *node = prefix##_lower_bound(head, key); \
        if (node && macro_equal_kv(macro_skiplist_default(), key_type, value_type, cmp, key, node)) return node; \
        return NULL; \
    } \
    MOD _macro_skiplist_kv_with_field(prefix##_upper_bound, field, upper_bound, macro_skiplist_default(), key_type, value_type, cmp) \
    MOD _macro_skiplist_kv_with_field(prefix##_floor, field, floor, macro_skiplist_default(), key_type, value_type, cmp) \
    MOD _macro_skiplist_kv_with_field(prefix##_ceiling, field, ceiling, macro_skiplist_default(), key_type, value_type, cmp) \
    MOD _macro_multi_skiplist_insert_with_field(prefix##_insert, field, macro_skiplist_default(), value_type, cmp)

/* ==========================================================================
 * PUBLIC GENERATORS
 * ========================================================================== */

/* Standard Skiplist APIs */
#define macro_skiplist_api_h(prefix, type)                            __macro_skiplist_api_h(, prefix, type)
#define macro_skiplist_static_api_h(prefix, type)                     __macro_skiplist_api_h(static, prefix, type)
#define macro_skiplist_static_inline_api_h(prefix, type)              __macro_skiplist_api_h(static inline, prefix, type)

#define macro_skiplist_api_h_with_field(prefix, field, type)               __macro_skiplist_api_h_with_field(, prefix, field, type)
#define macro_skiplist_static_api_h_with_field(prefix, field, type)        __macro_skiplist_api_h_with_field(static, prefix, field, type)
#define macro_skiplist_static_inline_api_h_with_field(prefix, field, type) __macro_skiplist_api_h_with_field(static inline, prefix, field, type)

#define macro_skiplist_api(prefix, type, cmp)                         __macro_skiplist_api(, prefix, type, cmp)
#define macro_skiplist_static_api(prefix, type, cmp)                  __macro_skiplist_api(static, prefix, type, cmp)
#define macro_skiplist_static_inline_api(prefix, type, cmp)           __macro_skiplist_api(static inline, prefix, type, cmp)

#define macro_skiplist_api_with_field(prefix, field, type, cmp)               __macro_skiplist_api_with_field(, prefix, field, type, cmp)
#define macro_skiplist_static_api_with_field(prefix, field, type, cmp)        __macro_skiplist_api_with_field(static, prefix, field, type, cmp)
#define macro_skiplist_static_inline_api_with_field(prefix, field, type, cmp) __macro_skiplist_api_with_field(static inline, prefix, field, type, cmp)

/* KV Skiplist APIs */
#define macro_skiplist_kv_api_h(prefix, key_t, val_t)                 __macro_skiplist_kv_api_h(, prefix, key_t, val_t)
#define macro_skiplist_static_kv_api_h(prefix, key_t, val_t)          __macro_skiplist_kv_api_h(static, prefix, key_t, val_t)
#define macro_skiplist_static_inline_kv_api_h(prefix, key_t, val_t)   __macro_skiplist_kv_api_h(static inline, prefix, key_t, val_t)

#define macro_skiplist_kv_api_h_with_field(prefix, f, key_t, val_t)               __macro_skiplist_kv_api_h_with_field(, prefix, f, key_t, val_t)
#define macro_skiplist_static_kv_api_h_with_field(prefix, f, key_t, val_t)        __macro_skiplist_kv_api_h_with_field(static, prefix, f, key_t, val_t)
#define macro_skiplist_static_inline_kv_api_h_with_field(prefix, f, key_t, val_t) __macro_skiplist_kv_api_h_with_field(static inline, prefix, f, key_t, val_t)

#define macro_skiplist_kv_api(prefix, key_t, val_t, cmp)              __macro_skiplist_kv_api(, prefix, key_t, val_t, cmp)
#define macro_skiplist_static_kv_api(prefix, key_t, val_t, cmp)       __macro_skiplist_kv_api(static, prefix, key_t, val_t, cmp)
#define macro_skiplist_static_inline_kv_api(prefix, key_t, val_t, cmp) __macro_skiplist_kv_api(static inline, prefix, key_t, val_t, cmp)

#define macro_skiplist_kv_api_with_field(prefix, f, key_t, val_t, cmp)               __macro_skiplist_kv_api_with_field(, prefix, f, key_t, val_t, cmp)
#define macro_skiplist_static_kv_api_with_field(prefix, f, key_t, val_t, cmp)        __macro_skiplist_kv_api_with_field(static, prefix, f, key_t, val_t, cmp)
#define macro_skiplist_static_inline_kv_api_with_field(prefix, f, key_t, val_t, cmp) __macro_skiplist_kv_api_with_field(static inline, prefix, f, key_t, val_t, cmp)

/* Multi Skiplist APIs */
#define macro_multi_skiplist_api_h(prefix, type)                            __macro_multi_skiplist_api_h(, prefix, type)
#define macro_multi_skiplist_static_api_h(prefix, type)                     __macro_multi_skiplist_api_h(static, prefix, type)
#define macro_multi_skiplist_static_inline_api_h(prefix, type)              __macro_multi_skiplist_api_h(static inline, prefix, type)

#define macro_multi_skiplist_api_h_with_field(prefix, field, type)               __macro_multi_skiplist_api_h_with_field(, prefix, field, type)
#define macro_multi_skiplist_static_api_h_with_field(prefix, field, type)        __macro_multi_skiplist_api_h_with_field(static, prefix, field, type)
#define macro_multi_skiplist_static_inline_api_h_with_field(prefix, field, type) __macro_multi_skiplist_api_h_with_field(static inline, prefix, field, type)

#define macro_multi_skiplist_api(prefix, type, cmp)                         __macro_multi_skiplist_api(, prefix, type, cmp)
#define macro_multi_skiplist_static_api(prefix, type, cmp)                  __macro_multi_skiplist_api(static, prefix, type, cmp)
#define macro_multi_skiplist_static_inline_api(prefix, type, cmp)           __macro_multi_skiplist_api(static inline, prefix, type, cmp)

#define macro_multi_skiplist_api_with_field(prefix, field, type, cmp)               __macro_multi_skiplist_api_with_field(, prefix, field, type, cmp)
#define macro_multi_skiplist_static_api_with_field(prefix, field, type, cmp)        __macro_multi_skiplist_api_with_field(static, prefix, field, type, cmp)
#define macro_multi_skiplist_static_inline_api_with_field(prefix, field, type, cmp) __macro_multi_skiplist_api_with_field(static inline, prefix, field, type, cmp)

/* KV Multi Skiplist APIs */
#define macro_multi_skiplist_kv_api_h(prefix, key_t, val_t)                 __macro_multi_skiplist_kv_api_h(, prefix, key_t, val_t)
#define macro_multi_skiplist_static_kv_api_h(prefix, key_t, val_t)          __macro_multi_skiplist_kv_api_h(static, prefix, key_t, val_t)
#define macro_multi_skiplist_static_inline_kv_api_h(prefix, key_t, val_t)   __macro_multi_skiplist_kv_api_h(static inline, prefix, key_t, val_t)

#define macro_multi_skiplist_kv_api_h_with_field(prefix, f, key_t, val_t)               __macro_multi_skiplist_kv_api_h_with_field(, prefix, f, key_t, val_t)
#define macro_multi_skiplist_static_kv_api_h_with_field(prefix, f, key_t, val_t)        __macro_multi_skiplist_kv_api_h_with_field(static, prefix, f, key_t, val_t)
#define macro_multi_skiplist_static_inline_kv_api_h_with_field(prefix, f, key_t, val_t) __macro_multi_skiplist_kv_api_h_with_field(static inline, prefix, f, key_t, val_t)

#define macro_multi_skiplist_kv_api(prefix, key_t, val_t, cmp)              __macro_multi_skiplist_kv_api(, prefix, key_t, val_t, cmp)
#define macro_multi_skiplist_static_kv_api(prefix, key_t, val_t, cmp)       __macro_multi_skiplist_kv_api(static, prefix, key_t, val_t, cmp)
#define macro_multi_skiplist_static_inline_kv_api(prefix, key_t, val_t, cmp) __macro_multi_skiplist_kv_api(static inline, prefix, key_t, val_t, cmp)

#define macro_multi_skiplist_kv_api_with_field(prefix, f, key_t, val_t, cmp)               __macro_multi_skiplist_kv_api_with_field(, prefix, f, key_t, val_t, cmp)
#define macro_multi_skiplist_static_kv_api_with_field(prefix, f, key_t, val_t, cmp)        __macro_multi_skiplist_kv_api_with_field(static, prefix, f, key_t, val_t, cmp)
#define macro_multi_skiplist_static_inline_kv_api_with_field(prefix, f, key_t, val_t, cmp) __macro_multi_skiplist_kv_api_with_field(static inline, prefix, f, key_t, val_t, cmp)

/* ==========================================================================
 * INTERNAL COMPARE GENERATORS (Linkage Parameterized)
 * ========================================================================== */

/* Standard Skiplist Compare */
#define __macro_skiplist_compare_api_h(MOD, prefix, type) \
    MOD _macro_skiplist_h(prefix##_find, compare_cmp_no_arg, type); \
    MOD _macro_skiplist_h(prefix##_lower_bound, compare_cmp_no_arg, type); \
    MOD _macro_skiplist_h(prefix##_upper_bound, compare_cmp_no_arg, type); \
    MOD _macro_skiplist_h(prefix##_floor, compare_cmp_no_arg, type); \
    MOD _macro_skiplist_h(prefix##_ceiling, compare_cmp_no_arg, type); \
    MOD _macro_skiplist_insert_h(prefix##_insert, compare_cmp_no_arg, type)

#define __macro_skiplist_compare_api_h_with_field(MOD, prefix, field, type) \
    MOD _macro_skiplist_h(prefix##_find, compare_cmp_no_arg, type); \
    MOD _macro_skiplist_h(prefix##_lower_bound, compare_cmp_no_arg, type); \
    MOD _macro_skiplist_h(prefix##_upper_bound, compare_cmp_no_arg, type); \
    MOD _macro_skiplist_h(prefix##_floor, compare_cmp_no_arg, type); \
    MOD _macro_skiplist_h(prefix##_ceiling, compare_cmp_no_arg, type); \
    MOD _macro_skiplist_insert_h(prefix##_insert, compare_cmp_no_arg, type)

#define __macro_skiplist_compare_api(MOD, prefix, type) \
    MOD _macro_skiplist(prefix##_find, find, compare_cmp_no_arg, type, cmp) \
    MOD _macro_skiplist(prefix##_lower_bound, lower_bound, compare_cmp_no_arg, type, cmp) \
    MOD _macro_skiplist(prefix##_upper_bound, upper_bound, compare_cmp_no_arg, type, cmp) \
    MOD _macro_skiplist(prefix##_floor, floor, compare_cmp_no_arg, type, cmp) \
    MOD _macro_skiplist(prefix##_ceiling, ceiling, compare_cmp_no_arg, type, cmp) \
    MOD _macro_skiplist_insert(prefix##_insert, compare_cmp_no_arg, type, cmp)

#define __macro_skiplist_compare_api_with_field(MOD, prefix, field, type) \
    MOD _macro_skiplist_with_field(prefix##_find, field, find, compare_cmp_no_arg, type, cmp) \
    MOD _macro_skiplist_with_field(prefix##_lower_bound, field, lower_bound, compare_cmp_no_arg, type, cmp) \
    MOD _macro_skiplist_with_field(prefix##_upper_bound, field, upper_bound, compare_cmp_no_arg, type, cmp) \
    MOD _macro_skiplist_with_field(prefix##_floor, field, floor, compare_cmp_no_arg, type, cmp) \
    MOD _macro_skiplist_with_field(prefix##_ceiling, field, ceiling, compare_cmp_no_arg, type, cmp) \
    MOD _macro_skiplist_insert_with_field(prefix##_insert, field, compare_cmp_no_arg, type, cmp)

/* KV Skiplist Compare */
#define __macro_skiplist_kv_compare_api_h(MOD, prefix, key_type, value_type) \
    MOD _macro_skiplist_kv_h(prefix##_find, compare_cmp_no_arg, key_type, value_type); \
    MOD _macro_skiplist_kv_h(prefix##_lower_bound, compare_cmp_no_arg, key_type, value_type); \
    MOD _macro_skiplist_kv_h(prefix##_upper_bound, compare_cmp_no_arg, key_type, value_type); \
    MOD _macro_skiplist_kv_h(prefix##_floor, compare_cmp_no_arg, key_type, value_type); \
    MOD _macro_skiplist_kv_h(prefix##_ceiling, compare_cmp_no_arg, key_type, value_type); \
    MOD _macro_skiplist_insert_h(prefix##_insert, compare_cmp_no_arg, value_type)

#define __macro_skiplist_kv_compare_api_h_with_field(MOD, prefix, field, key_type, value_type) \
    MOD _macro_skiplist_kv_h(prefix##_find, compare_cmp_no_arg, key_type, value_type); \
    MOD _macro_skiplist_kv_h(prefix##_lower_bound, compare_cmp_no_arg, key_type, value_type); \
    MOD _macro_skiplist_kv_h(prefix##_upper_bound, compare_cmp_no_arg, key_type, value_type); \
    MOD _macro_skiplist_kv_h(prefix##_floor, compare_cmp_no_arg, key_type, value_type); \
    MOD _macro_skiplist_kv_h(prefix##_ceiling, compare_cmp_no_arg, key_type, value_type); \
    MOD _macro_skiplist_insert_h(prefix##_insert, compare_cmp_no_arg, value_type)

#define __macro_skiplist_kv_compare_api(MOD, prefix, key_type, value_type) \
    MOD _macro_skiplist_kv(prefix##_find, find, compare_cmp_no_arg, key_type, value_type, cmp) \
    MOD _macro_skiplist_kv(prefix##_lower_bound, lower_bound, compare_cmp_no_arg, key_type, value_type, cmp) \
    MOD _macro_skiplist_kv(prefix##_upper_bound, upper_bound, compare_cmp_no_arg, key_type, value_type, cmp) \
    MOD _macro_skiplist_kv(prefix##_floor, floor, compare_cmp_no_arg, key_type, value_type, cmp) \
    MOD _macro_skiplist_kv(prefix##_ceiling, ceiling, compare_cmp_no_arg, key_type, value_type, cmp) \
    MOD _macro_skiplist_insert(prefix##_insert, compare_cmp_no_arg, value_type, cmp)

#define __macro_skiplist_kv_compare_api_with_field(MOD, prefix, field, key_type, value_type) \
    MOD _macro_skiplist_kv_with_field(prefix##_find, field, find, compare_cmp_no_arg, key_type, value_type, cmp) \
    MOD _macro_skiplist_kv_with_field(prefix##_lower_bound, field, lower_bound, compare_cmp_no_arg, key_type, value_type, cmp) \
    MOD _macro_skiplist_kv_with_field(prefix##_upper_bound, field, upper_bound, compare_cmp_no_arg, key_type, value_type, cmp) \
    MOD _macro_skiplist_kv_with_field(prefix##_floor, field, floor, compare_cmp_no_arg, key_type, value_type, cmp) \
    MOD _macro_skiplist_kv_with_field(prefix##_ceiling, field, ceiling, compare_cmp_no_arg, key_type, value_type, cmp) \
    MOD _macro_skiplist_insert_with_field(prefix##_insert, field, compare_cmp_no_arg, value_type, cmp)

/* Multi Skiplist Compare */
#define __macro_multi_skiplist_compare_api_h(MOD, prefix, type) \
    MOD _macro_skiplist_h(prefix##_lower_bound, compare_cmp_no_arg, type); \
    MOD _macro_skiplist_h(prefix##_find, compare_cmp_no_arg, type); \
    MOD _macro_skiplist_h(prefix##_upper_bound, compare_cmp_no_arg, type); \
    MOD _macro_skiplist_h(prefix##_floor, compare_cmp_no_arg, type); \
    MOD _macro_skiplist_h(prefix##_ceiling, compare_cmp_no_arg, type); \
    MOD _macro_skiplist_insert_h(prefix##_insert, compare_cmp_no_arg, type)

#define __macro_multi_skiplist_compare_api_h_with_field(MOD, prefix, field, type) \
    MOD _macro_skiplist_h(prefix##_lower_bound, compare_cmp_no_arg, type); \
    MOD _macro_skiplist_h(prefix##_find, compare_cmp_no_arg, type); \
    MOD _macro_skiplist_h(prefix##_upper_bound, compare_cmp_no_arg, type); \
    MOD _macro_skiplist_h(prefix##_floor, compare_cmp_no_arg, type); \
    MOD _macro_skiplist_h(prefix##_ceiling, compare_cmp_no_arg, type); \
    MOD _macro_skiplist_insert_h(prefix##_insert, compare_cmp_no_arg, type)

#define __macro_multi_skiplist_compare_api(MOD, prefix, type) \
    MOD _macro_skiplist(prefix##_lower_bound, lower_bound, compare_cmp_no_arg, type, cmp) \
    MOD _macro_skiplist_h(prefix##_find, compare_cmp_no_arg, type) { \
        type *node = prefix##_lower_bound(head, key, cmp); \
        if (node && macro_equal_cmp_no_arg(type, cmp, key, node)) return node; \
        return NULL; \
    } \
    MOD _macro_skiplist(prefix##_upper_bound, upper_bound, compare_cmp_no_arg, type, cmp) \
    MOD _macro_skiplist(prefix##_floor, floor, compare_cmp_no_arg, type, cmp) \
    MOD _macro_skiplist(prefix##_ceiling, ceiling, compare_cmp_no_arg, type, cmp) \
    MOD _macro_multi_skiplist_insert(prefix##_insert, compare_cmp_no_arg, type, cmp)

#define __macro_multi_skiplist_compare_api_with_field(MOD, prefix, field, type) \
    MOD _macro_skiplist_with_field(prefix##_lower_bound, field, lower_bound, compare_cmp_no_arg, type, cmp) \
    MOD _macro_skiplist_h(prefix##_find, compare_cmp_no_arg, type) { \
        type *node = prefix##_lower_bound(head, key, cmp); \
        if (node && macro_equal_cmp_no_arg(type, cmp, key, node)) return node; \
        return NULL; \
    } \
    MOD _macro_skiplist_with_field(prefix##_upper_bound, field, upper_bound, compare_cmp_no_arg, type, cmp) \
    MOD _macro_skiplist_with_field(prefix##_floor, field, floor, compare_cmp_no_arg, type, cmp) \
    MOD _macro_skiplist_with_field(prefix##_ceiling, field, ceiling, compare_cmp_no_arg, type, cmp) \
    MOD _macro_multi_skiplist_insert_with_field(prefix##_insert, field, compare_cmp_no_arg, type, cmp)

/* KV Multi Skiplist Compare */
#define __macro_multi_skiplist_kv_compare_api_h(MOD, prefix, key_type, value_type) \
    MOD _macro_skiplist_kv_h(prefix##_lower_bound, compare_cmp_no_arg, key_type, value_type); \
    MOD _macro_skiplist_kv_h(prefix##_find, compare_cmp_no_arg, key_type, value_type); \
    MOD _macro_skiplist_kv_h(prefix##_upper_bound, compare_cmp_no_arg, key_type, value_type); \
    MOD _macro_skiplist_kv_h(prefix##_floor, compare_cmp_no_arg, key_type, value_type); \
    MOD _macro_skiplist_kv_h(prefix##_ceiling, compare_cmp_no_arg, key_type, value_type); \
    MOD _macro_skiplist_insert_h(prefix##_insert, compare_cmp_no_arg, value_type)

#define __macro_multi_skiplist_kv_compare_api_h_with_field(MOD, prefix, field, key_type, value_type) \
    MOD _macro_skiplist_kv_h(prefix##_lower_bound, compare_cmp_no_arg, key_type, value_type); \
    MOD _macro_skiplist_kv_h(prefix##_find, compare_cmp_no_arg, key_type, value_type); \
    MOD _macro_skiplist_kv_h(prefix##_upper_bound, compare_cmp_no_arg, key_type, value_type); \
    MOD _macro_skiplist_kv_h(prefix##_floor, compare_cmp_no_arg, key_type, value_type); \
    MOD _macro_skiplist_kv_h(prefix##_ceiling, compare_cmp_no_arg, key_type, value_type); \
    MOD _macro_skiplist_insert_h(prefix##_insert, compare_cmp_no_arg, value_type)

#define __macro_multi_skiplist_kv_compare_api(MOD, prefix, key_type, value_type) \
    MOD _macro_skiplist_kv(prefix##_lower_bound, lower_bound, compare_cmp_no_arg, key_type, value_type, cmp) \
    MOD _macro_skiplist_kv_h(prefix##_find, compare_cmp_no_arg, key_type, value_type) { \
        value_type *node = prefix##_lower_bound(head, key, cmp); \
        if (node && macro_cmp_kv_cmp_no_arg(key_type, value_type, cmp, key, node) == 0) return node; \
        return NULL; \
    } \
    MOD _macro_skiplist_kv(prefix##_upper_bound, upper_bound, compare_cmp_no_arg, key_type, value_type, cmp) \
    MOD _macro_skiplist_kv(prefix##_floor, floor, compare_cmp_no_arg, key_type, value_type, cmp) \
    MOD _macro_skiplist_kv(prefix##_ceiling, ceiling, compare_cmp_no_arg, key_type, value_type, cmp) \
    MOD _macro_multi_skiplist_insert(prefix##_insert, compare_cmp_no_arg, value_type, cmp)

#define __macro_multi_skiplist_kv_compare_api_with_field(MOD, prefix, field, key_type, value_type) \
    MOD _macro_skiplist_kv_with_field(prefix##_lower_bound, field, lower_bound, compare_cmp_no_arg, key_type, value_type, cmp) \
    MOD _macro_skiplist_kv_h(prefix##_find, compare_cmp_no_arg, key_type, value_type) { \
        value_type *node = prefix##_lower_bound(head, key, cmp); \
        if (node && macro_cmp_kv_cmp_no_arg(key_type, value_type, cmp, key, node) == 0) return node; \
        return NULL; \
    } \
    MOD _macro_skiplist_kv_with_field(prefix##_upper_bound, field, upper_bound, compare_cmp_no_arg, key_type, value_type, cmp) \
    MOD _macro_skiplist_kv_with_field(prefix##_floor, field, floor, compare_cmp_no_arg, key_type, value_type, cmp) \
    MOD _macro_skiplist_kv_with_field(prefix##_ceiling, field, ceiling, compare_cmp_no_arg, key_type, value_type, cmp) \
    MOD _macro_multi_skiplist_insert_with_field(prefix##_insert, field, compare_cmp_no_arg, value_type, cmp)

/* ==========================================================================
 * PUBLIC COMPARE GENERATORS
 * ========================================================================== */

/* Standard Skiplist Compare APIs */
#define macro_skiplist_compare_api_h(prefix, type)                            __macro_skiplist_compare_api_h(, prefix, type)
#define macro_skiplist_static_compare_api_h(prefix, type)                     __macro_skiplist_compare_api_h(static, prefix, type)
#define macro_skiplist_static_inline_compare_api_h(prefix, type)              __macro_skiplist_compare_api_h(static inline, prefix, type)

#define macro_skiplist_compare_api_h_with_field(prefix, field, type)               __macro_skiplist_compare_api_h_with_field(, prefix, field, type)
#define macro_skiplist_static_compare_api_h_with_field(prefix, field, type)        __macro_skiplist_compare_api_h_with_field(static, prefix, field, type)
#define macro_skiplist_static_inline_compare_api_h_with_field(prefix, field, type) __macro_skiplist_compare_api_h_with_field(static inline, prefix, field, type)

#define macro_skiplist_compare_api(prefix, type)                         __macro_skiplist_compare_api(, prefix, type)
#define macro_skiplist_static_compare_api(prefix, type)                  __macro_skiplist_compare_api(static, prefix, type)
#define macro_skiplist_static_inline_compare_api(prefix, type)           __macro_skiplist_compare_api(static inline, prefix, type)

#define macro_skiplist_compare_api_with_field(prefix, field, type)               __macro_skiplist_compare_api_with_field(, prefix, field, type)
#define macro_skiplist_static_compare_api_with_field(prefix, field, type)        __macro_skiplist_compare_api_with_field(static, prefix, field, type)
#define macro_skiplist_static_inline_compare_api_with_field(prefix, field, type) __macro_skiplist_compare_api_with_field(static inline, prefix, field, type)

/* KV Skiplist Compare APIs */
#define macro_skiplist_kv_compare_api_h(prefix, key_t, val_t)                 __macro_skiplist_kv_compare_api_h(, prefix, key_t, val_t)
#define macro_skiplist_static_kv_compare_api_h(prefix, key_t, val_t)          __macro_skiplist_kv_compare_api_h(static, prefix, key_t, val_t)
#define macro_skiplist_static_inline_kv_compare_api_h(prefix, key_t, val_t)   __macro_skiplist_kv_compare_api_h(static inline, prefix, key_t, val_t)

#define macro_skiplist_kv_compare_api_h_with_field(prefix, f, key_t, val_t)               __macro_skiplist_kv_compare_api_h_with_field(, prefix, f, key_t, val_t)
#define macro_skiplist_static_kv_compare_api_h_with_field(prefix, f, key_t, val_t)        __macro_skiplist_kv_compare_api_h_with_field(static, prefix, f, key_t, val_t)
#define macro_skiplist_static_inline_kv_compare_api_h_with_field(prefix, f, key_t, val_t) __macro_skiplist_kv_compare_api_h_with_field(static inline, prefix, f, key_t, val_t)

#define macro_skiplist_kv_compare_api(prefix, key_t, val_t)              __macro_skiplist_kv_compare_api(, prefix, key_t, val_t)
#define macro_skiplist_static_kv_compare_api(prefix, key_t, val_t)       __macro_skiplist_kv_compare_api(static, prefix, key_t, val_t)
#define macro_skiplist_static_inline_kv_compare_api(prefix, key_t, val_t) __macro_skiplist_kv_compare_api(static inline, prefix, key_t, val_t)

#define macro_skiplist_kv_compare_api_with_field(prefix, f, key_t, val_t)               __macro_skiplist_kv_compare_api_with_field(, prefix, f, key_t, val_t)
#define macro_skiplist_static_kv_compare_api_with_field(prefix, f, key_t, val_t)        __macro_skiplist_kv_compare_api_with_field(static, prefix, f, key_t, val_t)
#define macro_skiplist_static_inline_kv_compare_api_with_field(prefix, f, key_t, val_t) __macro_skiplist_kv_compare_api_with_field(static inline, prefix, f, key_t, val_t)

/* Multi Skiplist Compare APIs */
#define macro_multi_skiplist_compare_api_h(prefix, type)                            __macro_multi_skiplist_compare_api_h(, prefix, type)
#define macro_multi_skiplist_static_compare_api_h(prefix, type)                     __macro_multi_skiplist_compare_api_h(static, prefix, type)
#define macro_multi_skiplist_static_inline_compare_api_h(prefix, type)              __macro_multi_skiplist_compare_api_h(static inline, prefix, type)

#define macro_multi_skiplist_compare_api_h_with_field(prefix, field, type)               __macro_multi_skiplist_compare_api_h_with_field(, prefix, field, type)
#define macro_multi_skiplist_static_compare_api_h_with_field(prefix, field, type)        __macro_multi_skiplist_compare_api_h_with_field(static, prefix, field, type)
#define macro_multi_skiplist_static_inline_compare_api_h_with_field(prefix, field, type) __macro_multi_skiplist_compare_api_h_with_field(static inline, prefix, field, type)

#define macro_multi_skiplist_compare_api(prefix, type)                         __macro_multi_skiplist_compare_api(, prefix, type)
#define macro_multi_skiplist_static_compare_api(prefix, type)                  __macro_multi_skiplist_compare_api(static, prefix, type)
#define macro_multi_skiplist_static_inline_compare_api(prefix, type)           __macro_multi_skiplist_compare_api(static inline, prefix, type)

#define macro_multi_skiplist_compare_api_with_field(prefix, field, type)               __macro_multi_skiplist_compare_api_with_field(, prefix, field, type)
#define macro_multi_skiplist_static_compare_api_with_field(prefix, field, type)        __macro_multi_skiplist_compare_api_with_field(static, prefix, field, type)
#define macro_multi_skiplist_static_inline_compare_api_with_field(prefix, field, type) __macro_multi_skiplist_compare_api_with_field(static inline, prefix, field, type)

/* KV Multi Skiplist Compare APIs */
#define macro_multi_skiplist_kv_compare_api_h(prefix, key_t, val_t)                 __macro_multi_skiplist_kv_compare_api_h(, prefix, key_t, val_t)
#define macro_multi_skiplist_static_kv_compare_api_h(prefix, key_t, val_t)          __macro_multi_skiplist_kv_compare_api_h(static, prefix, key_t, val_t)
#define macro_multi_skiplist_static_inline_kv_compare_api_h(prefix, key_t, val_t)   __macro_multi_skiplist_kv_compare_api_h(static inline, prefix, key_t, val_t)

#define macro_multi_skiplist_kv_compare_api_h_with_field(prefix, f, key_t, val_t)               __macro_multi_skiplist_kv_compare_api_h_with_field(, prefix, f, key_t, val_t)
#define macro_multi_skiplist_static_kv_compare_api_h_with_field(prefix, f, key_t, val_t)        __macro_multi_skiplist_kv_compare_api_h_with_field(static, prefix, f, key_t, val_t)
#define macro_multi_skiplist_static_inline_kv_compare_api_h_with_field(prefix, f, key_t, val_t) __macro_multi_skiplist_kv_compare_api_h_with_field(static inline, prefix, f, key_t, val_t)

#define macro_multi_skiplist_kv_compare_api(prefix, key_t, val_t)              __macro_multi_skiplist_kv_compare_api(, prefix, key_t, val_t)
#define macro_multi_skiplist_static_kv_compare_api(prefix, key_t, val_t)       __macro_multi_skiplist_kv_compare_api(static, prefix, key_t, val_t)
#define macro_multi_skiplist_static_inline_kv_compare_api(prefix, key_t, val_t) __macro_multi_skiplist_kv_compare_api(static inline, prefix, key_t, val_t)

#define macro_multi_skiplist_kv_compare_api_with_field(prefix, f, key_t, val_t)               __macro_multi_skiplist_kv_compare_api_with_field(, prefix, f, key_t, val_t)
#define macro_multi_skiplist_static_kv_compare_api_with_field(prefix, f, key_t, val_t)        __macro_multi_skiplist_kv_compare_api_with_field(static, prefix, f, key_t, val_t)
#define macro_multi_skiplist_static_inline_kv_compare_api_with_field(prefix, f, key_t, val_t) __macro_multi_skiplist_kv_compare_api_with_field(static inline, prefix, f, key_t, val_t)

#endif /* _macro_skiplist_H */
