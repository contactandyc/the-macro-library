// SPDX-FileCopyrightText: 2019–2026 Andy Curtis <contactandyc@gmail.com>
// SPDX-FileCopyrightText: 2024–2025 Knode.ai
// SPDX-License-Identifier: Apache-2.0
//
// Maintainer: Andy Curtis <contactandyc@gmail.com>

#ifndef _macro_topk_H
#define _macro_topk_H

#include "the-macro-library/macro_heap.h"
#include <stdbool.h>

/* --------------------------------------------------------------------------
 * TYPES
 * -------------------------------------------------------------------------- */
#define macro_topk_t(name, type) macro_heap_t(name, type)
#define macro_bottomk_t(name, type) macro_topk_t(name, type)

/*
 * We define custom bool-returning early exits that safely mirror
 * macro_check_min_heap_zero (which is hardcoded to return void).
 */
#define __macro_check_min_heap_zero_bool(style, type, cmp, arr, num) \
    if ((num) <= 1) return true; \
    if (!macro_less(style, type, cmp, (arr) + 1, (arr)) && \
        ((num) <= 2 || !macro_less(style, type, cmp, (arr) + 2, (arr)))) return true

#define __macro_check_max_heap_zero_bool(style, type, cmp, arr, num) \
    if ((num) <= 1) return true; \
    if (!macro_less(style, type, cmp, (arr), (arr) + 1) && \
        ((num) <= 2 || !macro_less(style, type, cmp, (arr), (arr) + 2))) return true

/* --------------------------------------------------------------------------
 * PUSH API
 * -------------------------------------------------------------------------- */
#define macro_topk_push_h(name, style, type, heap_type) \
    bool name(macro_cmp_signature(heap_type* ht, style, type), type item)

#define macro_bottomk_push_h(name, style, type, heap_type) \
    macro_topk_push_h(name, style, type, heap_type)

#define _macro_topk_push(name, style, type, cmp, heap_type, heap_style, is_bottom) \
    macro_topk_push_h(name, style, type, heap_type) { \
        if (ht->num < ht->size) { \
            ht->arr[ht->num] = item; \
            type tmp; \
            __macro_##heap_style##_heap_push_code(style, type, cmp, ht->arr, ht->num); \
            return true; \
        } \
        if (is_bottom ? macro_less(style, type, cmp, &item, ht->arr) : macro_less(style, type, cmp, ht->arr, &item)) { \
            ht->arr[0] = item; \
            __macro_check_##heap_style##_heap_zero_bool(style, type, cmp, ht->arr, ht->num); \
            __macro_##heap_style##_heap_pop_code(style, type, cmp, ht->arr, ht->num); \
            __macro_##heap_style##_heap_push_code(style, type, cmp, ht->arr, ht->num); \
            return true; \
        } \
        return false; \
    }

/* --------------------------------------------------------------------------
 * PEEK API (Fast Path - Caller must ensure num > 0)
 * -------------------------------------------------------------------------- */
#define macro_topk_peek_h(name, type, heap_type) \
    type* name(const heap_type* ht)

#define macro_bottomk_peek_h(name, type, heap_type) \
    macro_topk_peek_h(name, type, heap_type)

#define macro_topk_peek(name, type, heap_type) \
    macro_topk_peek_h(name, type, heap_type) { \
        return &ht->arr[0]; \
    }

#define macro_bottomk_peek(name, type, heap_type) macro_topk_peek(name, type, heap_type)

/* --------------------------------------------------------------------------
 * POP API (Fast Path - Caller must ensure num > 0)
 * -------------------------------------------------------------------------- */
#define macro_topk_pop_h(name, style, type, heap_type) \
    type* name(macro_cmp_signature(heap_type* ht, style, type))

#define macro_bottomk_pop_h(name, style, type, heap_type) \
    macro_topk_pop_h(name, style, type, heap_type)

#define _macro_topk_pop(name, style, type, cmp, heap_type, heap_style) \
    macro_topk_pop_h(name, style, type, heap_type) { \
        __macro_##heap_style##_heap_pop_code(style, type, cmp, ht->arr, ht->num); \
        return ht->arr + ht->num; \
    }

/* --------------------------------------------------------------------------
 * DEFAULT MACROS (Using cmp_no_arg / less_no_arg)
 * -------------------------------------------------------------------------- */
#define macro_topk_default() less_no_arg

/* Standard Push */
#define macro_topk_push_styled(name, style, type, cmp, heap_type) \
    _macro_topk_push(name, style, type, cmp, heap_type, min, false)

#define macro_bottomk_push_styled(name, style, type, cmp, heap_type) \
    _macro_topk_push(name, style, type, cmp, heap_type, max, true)

#define macro_topk_push(name, type, cmp, heap_type) \
    _macro_topk_push(name, macro_topk_default(), type, cmp, heap_type, min, false)

#define macro_bottomk_push(name, type, cmp, heap_type) \
    _macro_topk_push(name, macro_topk_default(), type, cmp, heap_type, max, true)

/* Standard Pop */
#define macro_topk_pop_styled(name, style, type, cmp, heap_type) \
    _macro_topk_pop(name, style, type, cmp, heap_type, min)

#define macro_bottomk_pop_styled(name, style, type, cmp, heap_type) \
    _macro_topk_pop(name, style, type, cmp, heap_type, max)

#define macro_topk_pop(name, type, cmp, heap_type) \
    _macro_topk_pop(name, macro_topk_default(), type, cmp, heap_type, min)

#define macro_bottomk_pop(name, type, cmp, heap_type) \
    _macro_topk_pop(name, macro_topk_default(), type, cmp, heap_type, max)

/* --------------------------------------------------------------------------
 * COMPARE MACROS (For function pointers)
 * -------------------------------------------------------------------------- */
#define macro_topk_push_compare_h(name, style, type, heap_type) \
    bool name(macro_cmp_signature(heap_type* ht, compare_ ## style, type), type item)

#define macro_bottomk_push_compare_h(name, style, type, heap_type) \
    macro_topk_push_compare_h(name, style, type, heap_type)

#define _macro_topk_push_compare(name, style, type, heap_type, heap_style, is_bottom) \
    macro_topk_push_compare_h(name, style, type, heap_type) { \
        if (ht->num < ht->size) { \
            ht->arr[ht->num] = item; \
            type tmp; \
            __macro_##heap_style##_heap_push_code(style, type, cmp, ht->arr, ht->num); \
            return true; \
        } \
        if (is_bottom ? macro_less(style, type, cmp, &item, ht->arr) : macro_less(style, type, cmp, ht->arr, &item)) { \
            ht->arr[0] = item; \
            __macro_check_##heap_style##_heap_zero_bool(style, type, cmp, ht->arr, ht->num); \
            __macro_##heap_style##_heap_pop_code(style, type, cmp, ht->arr, ht->num); \
            __macro_##heap_style##_heap_push_code(style, type, cmp, ht->arr, ht->num); \
            return true; \
        } \
        return false; \
    }

#define macro_topk_push_compare(name, type, heap_type) \
    _macro_topk_push_compare(name, macro_topk_default(), type, heap_type, min, false)

#define macro_bottomk_push_compare(name, type, heap_type) \
    _macro_topk_push_compare(name, macro_topk_default(), type, heap_type, max, true)

#define macro_topk_pop_compare_h(name, style, type, heap_type) \
    type* name(macro_cmp_signature(heap_type* ht, compare_ ## style, type))

#define macro_bottomk_pop_compare_h(name, style, type, heap_type) \
    macro_topk_pop_compare_h(name, style, type, heap_type)

#define _macro_topk_pop_compare(name, style, type, heap_type, heap_style) \
    macro_topk_pop_compare_h(name, style, type, heap_type) { \
        __macro_##heap_style##_heap_pop_code(style, type, cmp, ht->arr, ht->num); \
        return ht->arr + ht->num; \
    }

#define macro_topk_pop_compare(name, type, heap_type) \
    _macro_topk_pop_compare(name, macro_topk_default(), type, heap_type, min)

#define macro_bottomk_pop_compare(name, type, heap_type) \
    _macro_topk_pop_compare(name, macro_topk_default(), type, heap_type, max)

/* ==========================================================================
 * INTERNAL GENERATOR ENGINE (Linkage Parameterized)
 * ========================================================================== */

/* Top-K */
#define __macro_topk_api_h(MOD, prefix, type, heap_type) \
    MOD macro_topk_push_h(prefix##_push, macro_topk_default(), type, heap_type); \
    MOD macro_topk_peek_h(prefix##_peek, type, heap_type); \
    MOD macro_topk_pop_h(prefix##_pop, macro_topk_default(), type, heap_type)

#define __macro_topk_api(MOD, prefix, type, cmp, heap_type) \
    MOD macro_topk_push(prefix##_push, type, cmp, heap_type) \
    MOD macro_topk_peek(prefix##_peek, type, heap_type) \
    MOD macro_topk_pop(prefix##_pop, type, cmp, heap_type)

/* Bottom-K */
#define __macro_bottomk_api_h(MOD, prefix, type, heap_type) \
    MOD macro_bottomk_push_h(prefix##_push, macro_topk_default(), type, heap_type); \
    MOD macro_bottomk_peek_h(prefix##_peek, type, heap_type); \
    MOD macro_bottomk_pop_h(prefix##_pop, macro_topk_default(), type, heap_type)

#define __macro_bottomk_api(MOD, prefix, type, cmp, heap_type) \
    MOD macro_bottomk_push(prefix##_push, type, cmp, heap_type) \
    MOD macro_bottomk_peek(prefix##_peek, type, heap_type) \
    MOD macro_bottomk_pop(prefix##_pop, type, cmp, heap_type)

/* Top-K Compare */
#define __macro_topk_compare_api_h(MOD, prefix, type, heap_type) \
    MOD macro_topk_push_compare_h(prefix##_push, macro_topk_default(), type, heap_type); \
    MOD macro_topk_peek_h(prefix##_peek, type, heap_type); \
    MOD macro_topk_pop_compare_h(prefix##_pop, macro_topk_default(), type, heap_type)

#define __macro_topk_compare_api(MOD, prefix, type, heap_type) \
    MOD macro_topk_push_compare(prefix##_push, type, heap_type) \
    MOD macro_topk_peek(prefix##_peek, type, heap_type) \
    MOD macro_topk_pop_compare(prefix##_pop, type, heap_type)

/* Bottom-K Compare */
#define __macro_bottomk_compare_api_h(MOD, prefix, type, heap_type) \
    MOD macro_bottomk_push_compare_h(prefix##_push, macro_topk_default(), type, heap_type); \
    MOD macro_bottomk_peek_h(prefix##_peek, type, heap_type); \
    MOD macro_bottomk_pop_compare_h(prefix##_pop, macro_topk_default(), type, heap_type)

#define __macro_bottomk_compare_api(MOD, prefix, type, heap_type) \
    MOD macro_bottomk_push_compare(prefix##_push, type, heap_type) \
    MOD macro_bottomk_peek(prefix##_peek, type, heap_type) \
    MOD macro_bottomk_pop_compare(prefix##_pop, type, heap_type)

/* ==========================================================================
 * PUBLIC GENERATORS
 * ========================================================================== */

/* Top-K APIs */
#define macro_topk_api_h(prefix, type, heap_type)                  __macro_topk_api_h(, prefix, type, heap_type)
#define macro_topk_static_api_h(prefix, type, heap_type)           __macro_topk_api_h(static, prefix, type, heap_type)
#define macro_topk_static_inline_api_h(prefix, type, heap_type)    __macro_topk_api_h(static inline, prefix, type, heap_type)

#define macro_topk_api(prefix, type, cmp, heap_type)               __macro_topk_api(, prefix, type, cmp, heap_type)
#define macro_topk_static_api(prefix, type, cmp, heap_type)        __macro_topk_api(static, prefix, type, cmp, heap_type)
#define macro_topk_static_inline_api(prefix, type, cmp, heap_type) __macro_topk_api(static inline, prefix, type, cmp, heap_type)

/* Bottom-K APIs */
#define macro_bottomk_api_h(prefix, type, heap_type)                  __macro_bottomk_api_h(, prefix, type, heap_type)
#define macro_bottomk_static_api_h(prefix, type, heap_type)           __macro_bottomk_api_h(static, prefix, type, heap_type)
#define macro_bottomk_static_inline_api_h(prefix, type, heap_type)    __macro_bottomk_api_h(static inline, prefix, type, heap_type)

#define macro_bottomk_api(prefix, type, cmp, heap_type)               __macro_bottomk_api(, prefix, type, cmp, heap_type)
#define macro_bottomk_static_api(prefix, type, cmp, heap_type)        __macro_bottomk_api(static, prefix, type, cmp, heap_type)
#define macro_bottomk_static_inline_api(prefix, type, cmp, heap_type) __macro_bottomk_api(static inline, prefix, type, cmp, heap_type)

/* Top-K Compare APIs */
#define macro_topk_compare_api_h(prefix, type, heap_type)                  __macro_topk_compare_api_h(, prefix, type, heap_type)
#define macro_topk_static_compare_api_h(prefix, type, heap_type)           __macro_topk_compare_api_h(static, prefix, type, heap_type)
#define macro_topk_static_inline_compare_api_h(prefix, type, heap_type)    __macro_topk_compare_api_h(static inline, prefix, type, heap_type)

#define macro_topk_compare_api(prefix, type, heap_type)               __macro_topk_compare_api(, prefix, type, heap_type)
#define macro_topk_static_compare_api(prefix, type, heap_type)        __macro_topk_compare_api(static, prefix, type, heap_type)
#define macro_topk_static_inline_compare_api(prefix, type, heap_type) __macro_topk_compare_api(static inline, prefix, type, heap_type)

/* Bottom-K Compare APIs */
#define macro_bottomk_compare_api_h(prefix, type, heap_type)                  __macro_bottomk_compare_api_h(, prefix, type, heap_type)
#define macro_bottomk_static_compare_api_h(prefix, type, heap_type)           __macro_bottomk_compare_api_h(static, prefix, type, heap_type)
#define macro_bottomk_static_inline_compare_api_h(prefix, type, heap_type)    __macro_bottomk_compare_api_h(static inline, prefix, type, heap_type)

#define macro_bottomk_compare_api(prefix, type, heap_type)               __macro_bottomk_compare_api(, prefix, type, heap_type)
#define macro_bottomk_static_compare_api(prefix, type, heap_type)        __macro_bottomk_compare_api(static, prefix, type, heap_type)
#define macro_bottomk_static_inline_compare_api(prefix, type, heap_type) __macro_bottomk_compare_api(static inline, prefix, type, heap_type)

#endif /* _macro_topk_H */
