#include <stdio.h>

#include "macro_cmp.h"

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

#define __macro_bsearch_floor_code(style, value_type, cmp, key, base, n)    \
    __macro_bsearch_vars(value_type, base, n);                              \
    hi--;                                                                   \
    value_type *floor = NULL;                                               \
    while(lo <= hi) {                                                       \
        mid = lo + ((hi - lo) >> 1);                                        \
        int n = macro_cmp(style, value_type, cmp, key, mid);                \
        if (n < 0)  {                                                       \
            hi = mid - 1;                                                   \
        } else if(n > 0) {                                                  \
            lo = mid + 1;                                                   \
        } else {                                                            \
            floor = mid;                                                    \
            hi = mid - 1;                                                   \
        }                                                                   \
    }                                                                       \
    if (floor == NULL && lo > (value_type *)base) {                         \
        floor = lo - 1;                                                     \
    }                                                                       \
    return floor;

#define __macro_bsearch_kv_floor_code(style, key_type, value_type, cmp, key, base, n)    \
    __macro_bsearch_vars(value_type, base, n);                                           \
    hi--;                                                                                \
    value_type *floor = NULL;                                                            \
    while(lo <= hi) {                                                                    \
        mid = lo + ((hi - lo) >> 1);                                                     \
        int n = macro_cmp_kv(style, key_type, value_type, cmp, key, mid);                \
        if (n < 0)  {                                                                    \
            hi = mid - 1;                                                                \
        } else if(n > 0) {                                                               \
            lo = mid + 1;                                                                \
        } else {                                                                         \
            floor = mid;                                                                 \
            hi = mid - 1;                                                                \
        }                                                                                \
    }                                                                                    \
    if (floor == NULL && lo > (value_type *)base) {                                      \
        floor = lo - 1;                                                                  \
    }                                                                                    \
    return floor;

#define __macro_bsearch_ceiling_code(style, value_type, cmp, key, base, n)    \
    __macro_bsearch_vars(value_type, base, n);                                \
    hi--;                                                                     \
    value_type *ceiling = NULL;                                               \
    while(lo <= hi) {                                                         \
        mid = lo + ((hi - lo) >> 1);                                          \
        int n = macro_cmp(style, value_type, cmp, key, mid);                  \
        if (n < 0)  {                                                         \
            hi = mid - 1;                                                     \
        } else if(n > 0)                                                      \
            lo = mid + 1;                                                     \
        else {                                                                \
            ceiling = mid;                                                    \
            lo = mid + 1;                                                     \
        }                                                                     \
    }                                                                         \
    if (ceiling == NULL && hi >= (value_type *)base) {                        \
        ceiling = hi;                                                         \
    }                                                                         \
    return ceiling;

#define __macro_bsearch_kv_ceiling_code(style, key_type, value_type, cmp, key, base, n)    \
    __macro_bsearch_vars(value_type, base, n);                                             \
    hi--;                                                                                  \
    value_type *ceiling = NULL;                                                            \
    while(lo <= hi) {                                                                      \
        mid = lo + ((hi - lo) >> 1);                                                       \
        int n = macro_cmp_kv(style, key_type, value_type, cmp, key, mid);                  \
        if (n < 0)  {                                                                      \
            hi = mid - 1;                                                                  \
        } else if(n > 0)                                                                   \
            lo = mid + 1;                                                                  \
        else {                                                                             \
            ceiling = mid;                                                                 \
            lo = mid + 1;                                                                  \
        }                                                                                  \
    }                                                                                      \
    if (ceiling == NULL && hi >= (value_type *)base) {                                     \
        ceiling = hi;                                                                      \
    }                                                                                      \
    return ceiling;

#define __macro_bsearch_lower_bound_code(style, value_type, cmp, key, base, n)    \
    __macro_bsearch_vars(value_type, base, n);                                    \
    value_type *high = hi;                                                        \
    while(lo < hi) {                                                              \
        mid = lo + ((hi - lo) >> 1);                                              \
        if (macro_greater_kv(style, value_type, cmp, key, mid))                   \
            lo = mid + 1;                                                         \
        else                                                                      \
            hi = mid;                                                             \
    }                                                                             \
    return lo < high ? lo : NULL;

#define __macro_bsearch_kv_lower_bound_code(style, key_type, value_type, cmp, key, base, n)    \
    __macro_bsearch_vars(value_type, base, n);                                                 \
    value_type *high = hi;                                                                     \
    while(lo < hi) {                                                                           \
        mid = lo + ((hi - lo) >> 1);                                                           \
        if (macro_greater_kv(style, key_type, value_type, cmp, key, mid))                      \
            lo = mid + 1;                                                                      \
        else                                                                                   \
            hi = mid;                                                                          \
    }                                                                                          \
    return lo < high ? lo : NULL;

#define __macro_bsearch_upper_bound_code(style, value_type, cmp, key, base, n)    \
    __macro_bsearch_vars(value_type, base, n);                                    \
    while(lo < hi) {                                                              \
        mid = lo + ((hi - lo) >> 1);                                              \
        if (macro_less(style, value_type, cmp, key, mid))                         \
            hi = mid;                                                             \
        else                                                                      \
            lo = mid + 1;                                                         \
    }                                                                             \
    return lo;

#define __macro_bsearch_kv_upper_bound_code(style, key_type, value_type, cmp, key, base, n)    \
    __macro_bsearch_vars(value_type, base, n);                                                 \
    while(lo < hi) {                                                                           \
        mid = lo + ((hi - lo) >> 1);                                                           \
        if (macro_less_kv(style, key_type, value_type, cmp, key, mid))                         \
            hi = mid;                                                                          \
        else                                                                                   \
            lo = mid + 1;                                                                      \
    }                                                                                          \
    return lo;
