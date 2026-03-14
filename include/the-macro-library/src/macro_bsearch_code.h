// SPDX-FileCopyrightText: 2019–2026 Andy Curtis <contactandyc@gmail.com>
// SPDX-FileCopyrightText: 2024–2025 Knode.ai — technical questions: contact Andy (above)
// SPDX-License-Identifier: Apache-2.0

#include <stdio.h>

#include "the-macro-library/macro_cmp.h"

/* TODO: consider reducing the amount of repeated logic as was done in macro_map.h */

#define __macro_bsearch_vars(value_type, base, n)    \
    if(!n) return NULL;                              \
    value_type *lo = (value_type *)base;             \
    value_type *hi = lo + n;                         \
    value_type *mid

#define __macro_bsearch_core_code(style, value_type, cmp, key, base, n)    \
    __macro_bsearch_vars(value_type, base, n);                             \
    while(lo < hi) {                                                       \
        mid = lo + ((hi - lo) >> 1);                                       \
        int n = macro_cmp(style, value_type, cmp, key, mid);               \
        if (n > 0)                                                         \
            lo = mid + 1;                                                  \
        else if(n < 0)                                                     \
            hi = mid;                                                      \
        else                                                               \
            return mid;                                                    \
    }                                                                      \
    return NULL;

#define __macro_bsearch_kv_core_code(style, key_type, value_type, cmp, key, base, n)    \
    __macro_bsearch_vars(value_type, base, n);                                          \
    while(lo < hi) {                                                                    \
        mid = lo + ((hi - lo) >> 1);                                                    \
        int n = macro_cmp_kv(style, key_type, value_type, cmp, key, mid);               \
        if (n > 0)                                                                      \
            lo = mid + 1;                                                               \
        else if(n < 0)                                                                  \
            hi = mid;                                                                   \
        else                                                                            \
            return mid;                                                                 \
    }                                                                                   \
    return NULL;

#define __macro_bsearch_first_code(style, value_type, cmp, key, base, n)    \
    __macro_bsearch_vars(value_type, base, n);                              \
    value_type *high = hi;                                                  \
    while(lo < hi) {                                                        \
        mid = lo + ((hi - lo) >> 1);                                        \
        if (macro_greater(style, value_type, cmp, key, mid))                \
            lo = mid + 1;                                                   \
        else                                                                \
            hi = mid;                                                       \
    }                                                                       \
    if(lo != high && macro_equal(style, value_type, cmp, key, lo))          \
        return lo;                                                          \
    return NULL;

#define __macro_bsearch_kv_first_code(style, key_type, value_type, cmp, key, base, n)    \
    __macro_bsearch_vars(value_type, base, n);                                           \
    value_type *high = hi;                                                               \
    while(lo < hi) {                                                                     \
        mid = lo + ((hi - lo) >> 1);                                                     \
        if (macro_greater_kv(style, key_type, value_type, cmp, key, mid))                \
            lo = mid + 1;                                                                \
        else                                                                             \
            hi = mid;                                                                    \
    }                                                                                    \
    if(lo != high && macro_equal_kv(style, key_type, value_type, cmp, key, lo))          \
        return lo;                                                                       \
    return NULL;

#define __macro_bsearch_last_code(style, value_type, cmp, key, base, n)    \
    __macro_bsearch_vars(value_type, base, n);                             \
    value_type *low = lo;                                                  \
    while(lo < hi) {                                                       \
        mid = lo + ((hi - lo) >> 1);                                       \
        if (macro_less(style, value_type, cmp, key, mid))                  \
            hi = mid;                                                      \
        else                                                               \
            lo = mid + 1;                                                  \
    }                                                                      \
    lo--;                                                                  \
    if(lo >= low && macro_equal(style, value_type, cmp, key, lo))          \
        return lo;                                                         \
    return NULL;

#define __macro_bsearch_kv_last_code(style, key_type, value_type, cmp, key, base, n)    \
    __macro_bsearch_vars(value_type, base, n);                                          \
    value_type *low = lo;                                                               \
    while(lo < hi) {                                                                    \
        mid = lo + ((hi - lo) >> 1);                                                    \
        if (macro_less_kv(style, key_type, value_type, cmp, key, mid))                  \
            hi = mid;                                                                   \
        else                                                                            \
            lo = mid + 1;                                                               \
    }                                                                                   \
    lo--;                                                                               \
    if(lo >= low && macro_equal_kv(style, key_type, value_type, cmp, key, lo))          \
        return lo;                                                                      \
    return NULL;

/* Last element <= key */
#define __macro_bsearch_floor_code(style, value_type, cmp, key, base, n)    \
    __macro_bsearch_vars(value_type, base, n);                              \
    hi--;                                                                   \
    value_type *best = NULL;                                                \
    while (lo <= hi) {                                                      \
        mid = lo + ((hi - lo) >> 1);                                        \
        int c = macro_cmp(style, value_type, cmp, key, mid);                \
        if (c < 0) {            /* key < *mid */                            \
            hi = mid - 1;                                                   \
        } else {                   /* key >= *mid */                        \
            best = mid;                                                     \
            lo = mid + 1;                                                   \
        }                                                                   \
    }                                                                       \
    return best;

/* Last element with value->key <= key */
#define __macro_bsearch_kv_floor_code(style, key_type, value_type, cmp, key, base, n)    \
    __macro_bsearch_vars(value_type, base, n);                                           \
    hi--;                                                                                \
    value_type *best = NULL;                                                             \
    while (lo <= hi) {                                                                   \
        mid = lo + ((hi - lo) >> 1);                                                     \
        int c = macro_cmp_kv(style, key_type, value_type, cmp, key, mid);                \
        if (c < 0) {            /* key < value(mid) */                                   \
            hi = mid - 1;                                                                \
        } else {                   /* key >= value(mid) */                               \
            best = mid;                                                                  \
            lo = mid + 1;                                                                \
        }                                                                                \
    }                                                                                    \
    return best;

/* First element >= key */
#define __macro_bsearch_ceiling_code(style, value_type, cmp, key, base, n)    \
    __macro_bsearch_vars(value_type, base, n);                                \
    hi--;                                                                     \
    value_type *best = NULL;                                                  \
    while (lo <= hi) {                                                        \
        mid = lo + ((hi - lo) >> 1);                                          \
        int c = macro_cmp(style, value_type, cmp, key, mid);                  \
        if (c > 0) {            /* key > *mid */                              \
            lo = mid + 1;                                                     \
        } else {                   /* key <= *mid */                          \
            best = mid;                                                       \
            hi = mid - 1;                                                     \
        }                                                                     \
    }                                                                         \
    return best;


/* First element with value->key >= key */
#define __macro_bsearch_kv_ceiling_code(style, key_type, value_type, cmp, key, base, n)    \
    __macro_bsearch_vars(value_type, base, n);                                             \
    hi--;                                                                                  \
    value_type *best = NULL;                                                               \
    while (lo <= hi) {                                                                     \
        mid = lo + ((hi - lo) >> 1);                                                       \
        int c = macro_cmp_kv(style, key_type, value_type, cmp, key, mid);                  \
        if (c > 0) {            /* key > value(mid) */                                     \
            lo = mid + 1;                                                                  \
        } else {                   /* key <= value(mid) */                                 \
            best = mid;                                                                    \
            hi = mid - 1;                                                                  \
        }                                                                                  \
    }                                                                                      \
    return best;

#define __macro_bsearch_lower_bound_code(style, value_type, cmp, key, base, n)    \
    __macro_bsearch_vars(value_type, base, n);                                    \
    value_type *end = hi;                                                         \
    while (lo < hi) {                                                             \
        mid = lo + ((hi - lo) >> 1);                                              \
        if (macro_greater(style, value_type, cmp, key, mid))                      \
            lo = mid + 1;                                                         \
        else                                                                      \
            hi = mid;                                                             \
    }                                                                             \
    return (lo < end) ? lo : NULL;  /* NULL if past-the-end */

#define __macro_bsearch_kv_lower_bound_code(style, key_type, value_type, cmp, key, base, n)    \
    __macro_bsearch_vars(value_type, base, n);                                                 \
    value_type *end = hi;                                                                      \
    while (lo < hi) {                                                                          \
        mid = lo + ((hi - lo) >> 1);                                                           \
        if (macro_greater_kv(style, key_type, value_type, cmp, key, mid))                      \
            lo = mid + 1;                                                                      \
        else                                                                                   \
            hi = mid;                                                                          \
    }                                                                                          \
    return (lo < end) ? lo : NULL;

#define __macro_bsearch_upper_bound_code(style, value_type, cmp, key, base, n)    \
    __macro_bsearch_vars(value_type, base, n);                                    \
    value_type *end = hi;                                                         \
    while (lo < hi) {                                                             \
        mid = lo + ((hi - lo) >> 1);                                              \
        if (macro_less(style, value_type, cmp, key, mid))                         \
            hi = mid;                                                             \
        else                                                                      \
            lo = mid + 1;                                                         \
    }                                                                             \
    return (lo < end) ? lo : NULL;  /* NULL if past-the-end */


#define __macro_bsearch_kv_upper_bound_code(style, key_type, value_type, cmp, key, base, n)    \
    __macro_bsearch_vars(value_type, base, n);                                                 \
    value_type *end = hi;                                                                      \
    while (lo < hi) {                                                                          \
        mid = lo + ((hi - lo) >> 1);                                                           \
        if (macro_less_kv(style, key_type, value_type, cmp, key, mid))                         \
            hi = mid;                                                                          \
        else                                                                                   \
            lo = mid + 1;                                                                      \
    }                                                                                          \
    return (lo < end) ? lo : NULL;
