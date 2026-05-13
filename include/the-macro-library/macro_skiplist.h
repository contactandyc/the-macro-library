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

#ifndef macro_parent_object
#define macro_parent_object(addr, base_type, field)    \
  ((base_type *)((char *)(addr) - offsetof(base_type, field)))
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
 * Insert API (Standard and Multimap)
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

#define _macro_multimap_skiplist_insert_with_field(name, field, style, type, cmp) \
    _macro_skiplist_insert_h(name, style, type) { \
        _macro_multimap_skiplist_insert_code_with_field(head, field, style, type, cmp, node); \
    }

#define _macro_multimap_skiplist_insert(name, style, type, cmp) \
    _macro_skiplist_insert_h(name, style, type) { \
        _macro_multimap_skiplist_insert_code(head, style, type, cmp, node); \
    }

/* --------------------------------------------------------------------------
 * Default API Macros (Using cmp_no_arg)
 * -------------------------------------------------------------------------- */
#define macro_skiplist_default() cmp_no_arg

#define macro_skiplist_find_with_field(name, field, type, cmp) _macro_skiplist_with_field(name, field, find, macro_skiplist_default(), type, cmp)
#define macro_skiplist_find(name, type, cmp) _macro_skiplist(name, find, macro_skiplist_default(), type, cmp)

#define macro_skiplist_lower_bound_with_field(name, field, type, cmp) _macro_skiplist_with_field(name, field, lower_bound, macro_skiplist_default(), type, cmp)
#define macro_skiplist_lower_bound(name, type, cmp) _macro_skiplist(name, lower_bound, macro_skiplist_default(), type, cmp)

#define macro_skiplist_upper_bound_with_field(name, field, type, cmp) _macro_skiplist_with_field(name, field, upper_bound, macro_skiplist_default(), type, cmp)
#define macro_skiplist_upper_bound(name, type, cmp) _macro_skiplist(name, upper_bound, macro_skiplist_default(), type, cmp)

#define macro_skiplist_floor_with_field(name, field, type, cmp) _macro_skiplist_with_field(name, field, floor, macro_skiplist_default(), type, cmp)
#define macro_skiplist_floor(name, type, cmp) _macro_skiplist(name, floor, macro_skiplist_default(), type, cmp)

#define macro_skiplist_ceiling_with_field(name, field, type, cmp) _macro_skiplist_with_field(name, field, ceiling, macro_skiplist_default(), type, cmp)
#define macro_skiplist_ceiling(name, type, cmp) _macro_skiplist(name, ceiling, macro_skiplist_default(), type, cmp)

#define macro_skiplist_find_kv_with_field(name, field, key_type, value_type, cmp) _macro_skiplist_kv_with_field(name, field, find, macro_skiplist_default(), key_type, value_type, cmp)
#define macro_skiplist_find_kv(name, key_type, value_type, cmp) _macro_skiplist_kv(name, find, macro_skiplist_default(), key_type, value_type, cmp)

#define macro_skiplist_lower_bound_kv_with_field(name, field, key_type, value_type, cmp) _macro_skiplist_kv_with_field(name, field, lower_bound, macro_skiplist_default(), key_type, value_type, cmp)
#define macro_skiplist_lower_bound_kv(name, key_type, value_type, cmp) _macro_skiplist_kv(name, lower_bound, macro_skiplist_default(), key_type, value_type, cmp)

#define macro_skiplist_upper_bound_kv_with_field(name, field, key_type, value_type, cmp) _macro_skiplist_kv_with_field(name, field, upper_bound, macro_skiplist_default(), key_type, value_type, cmp)
#define macro_skiplist_upper_bound_kv(name, key_type, value_type, cmp) _macro_skiplist_kv(name, upper_bound, macro_skiplist_default(), key_type, value_type, cmp)

#define macro_skiplist_floor_kv_with_field(name, field, key_type, value_type, cmp) _macro_skiplist_kv_with_field(name, field, floor, macro_skiplist_default(), key_type, value_type, cmp)
#define macro_skiplist_floor_kv(name, key_type, value_type, cmp) _macro_skiplist_kv(name, floor, macro_skiplist_default(), key_type, value_type, cmp)

#define macro_skiplist_ceiling_kv_with_field(name, field, key_type, value_type, cmp) _macro_skiplist_kv_with_field(name, field, ceiling, macro_skiplist_default(), key_type, value_type, cmp)
#define macro_skiplist_ceiling_kv(name, key_type, value_type, cmp) _macro_skiplist_kv(name, ceiling, macro_skiplist_default(), key_type, value_type, cmp)

#define macro_skiplist_insert_with_field(name, field, type, cmp) _macro_skiplist_insert_with_field(name, field, macro_skiplist_default(), type, cmp)
#define macro_skiplist_insert(name, type, cmp) _macro_skiplist_insert(name, macro_skiplist_default(), type, cmp)

#define macro_multimap_skiplist_insert_with_field(name, field, type, cmp) _macro_multimap_skiplist_insert_with_field(name, field, macro_skiplist_default(), type, cmp)
#define macro_multimap_skiplist_insert(name, type, cmp) _macro_multimap_skiplist_insert(name, macro_skiplist_default(), type, cmp)

#endif /* _macro_skiplist_H */
