/*
Copyright 2023 Andy Curtis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

/*
    macro_bsearch - regular binary search
    macro_bsearch_lower_bound - search for value <= to key
    macro_bsearch_lower_match - search for the least occurrence of key
    macro_bsearch_upper_bound - search for value >= key
    macro_bsearch_upper_match - search for the greatest occurrence of key

    cmp_no_arg, cmp_arg, arg_cmp supported

    macro_bsearch..._compare - also supported
*/

#include <stdio.h>

#include "macro_cmp.h"

#define __macro_bsearch_vars(value_type, base, n)    \
    if(!n) return NULL;                              \
    value_type *lo = (value_type *)base;             \
    value_type *hi = lo + n;                         \
    value_type *mid

#define __macro_bsearch_code(style, key_type, value_type, cmp, key, base, n)    \
    __macro_bsearch_vars(value_type, base, n);                                  \
    while(lo < hi) {                                                            \
        mid = lo + ((hi - lo) >> 1);                                            \
        int n = macro_cmp_kv(style, key_type, value_type, cmp, key, mid);       \
        if (n > 0)                                                              \
            lo = mid + 1;                                                       \
        else if(n < 0)                                                          \
            hi = mid;                                                           \
        else                                                                    \
            return mid;                                                         \
    }                                                                           \
    return NULL;

#define __macro_bsearch_floor_code(style, key_type, value_type, cmp, key, base, n)          \
    __macro_bsearch_vars(value_type, base, n);                                              \
    value_type *low = lo; \
    while(lo < hi) {                                                                        \
        mid = lo + ((hi - lo) >> 1);                                                        \
        if (macro_greater_kv(style, key_type, value_type, cmp, key, mid))                   \
            lo = mid + 1;                                                                   \
        else                                                                                \
            hi = mid;                                                                       \
    }                                                                                       \
    lo--;   \
    return lo >= low ? lo : NULL;


#define __macro_bsearch_lower_bound_code(style, key_type, value_type, cmp, key, base, n)    \
    __macro_bsearch_vars(value_type, base, n);                                              \
    value_type *high = hi; \
    while(lo < hi) {                                                                        \
        mid = lo + ((hi - lo) >> 1);                                                        \
        if (macro_greater_kv(style, key_type, value_type, cmp, key, mid))                   \
            lo = mid + 1;                                                                   \
        else                                                                                \
            hi = mid;                                                                       \
    }                                                                                       \
    return lo < high ? lo : NULL;

#define __macro_bsearch_first_code(style, key_type, value_type, cmp, key, base, n)    \
    __macro_bsearch_vars(value_type, base, n);                                              \
    value_type *high = hi;                                                                  \
    while(lo < hi) {                                                                        \
        mid = lo + ((hi - lo) >> 1);                                                        \
        if (macro_greater_kv(style, key_type, value_type, cmp, key, mid))                   \
            lo = mid + 1;                                                                   \
        else                                                                                \
            hi = mid;                                                                       \
    }                                                                                       \
    if(lo != high && macro_equal_kv(style, key_type, value_type, cmp, key, lo))             \
        return lo;                                                                          \
    return NULL;

#define __macro_bsearch_ceiling_code(style, key_type, value_type, cmp, key, base, n)        \
    __macro_bsearch_vars(value_type, base, n);                                              \
    value_type *high = hi; \
    while(lo < hi) {                                                                        \
        mid = lo + ((hi - lo) >> 1);                                                        \
        if (macro_less_kv(style, key_type, value_type, cmp, key, mid))                      \
            hi = mid;                                                                       \
        else                                                                                \
            lo = mid + 1;                                                                   \
    }                                                                                       \
    return lo <= high ? lo : NULL;

#define __macro_bsearch_upper_bound_code(style, key_type, value_type, cmp, key, base, n)    \
    __macro_bsearch_vars(value_type, base, n);                                              \
    value_type *low = lo; \
    while(lo < hi) {                                                                        \
        mid = lo + ((hi - lo) >> 1);                                                        \
        if (macro_less_kv(style, key_type, value_type, cmp, key, mid))                      \
            hi = mid;                                                                       \
        else                                                                                \
            lo = mid + 1;                                                                   \
    }                                                                                       \
    lo--;                                                                                   \
    return lo >= low ? lo : NULL;

#define __macro_bsearch_last_code(style, key_type, value_type, cmp, key, base, n)    \
    __macro_bsearch_vars(value_type, base, n);                                              \
    value_type *low = lo; \
    while(lo < hi) {                                                                        \
        mid = lo + ((hi - lo) >> 1);                                                        \
        if (macro_less_kv(style, key_type, value_type, cmp, key, mid))                      \
            hi = mid;                                                                       \
        else                                                                                \
            lo = mid + 1;                                                                   \
    }                                                                                       \
    lo--;                                                                                   \
    if(lo >= low && macro_equal_kv(style, key_type, value_type, cmp, key, lo))  \
        return lo;  \
    return NULL;


/* start macro_bsearch */

#define macro_bsearch_h_cmp_no_arg(name, key_type, value_type)    \
    value_type *name(const key_type *key, const value_type *base, size_t n)

#define macro_bsearch_cmp_no_arg(name, key_type, value_type, cmp)                     \
    value_type *name(const key_type *key, const value_type *base, size_t n) {         \
        __macro_bsearch_code(cmp_no_arg, key_type, value_type, cmp, key, base, n);    \
    }

#define __macro_bsearch_h_arg(name, key_type, value_type) value_type *name(const key_type *key, const value_type *base, size_t n, void *arg)

#define macro_bsearch_h_cmp_arg(name, key_type, value_type, cmp) __macro_bsearch_h_arg(name, key_type, value_type)
#define macro_bsearch_h_arg_cmp(name, key_type, value_type, cmp) __macro_bsearch_h_arg(name, key_type, value_type)

#define __macro_bsearch_arg(name, style, key_type, value_type, cmp)                         \
    value_type *name(const key_type *key, const value_type *base, size_t n, void *arg) {    \
        __macro_bsearch_code(style, key_type, value_type, cmp, key, base, n);               \
    }

#define macro_bsearch_cmp_arg(name, key_type, value_type, cmp) __macro_bsearch_arg(name, cmp_arg, key_type, value_type, cmp)
#define macro_bsearch_arg_cmp(name, key_type, value_type, cmp) __macro_bsearch_arg(name, arg_cmp, key_type, value_type, cmp)


#define macro_bsearch_compare_h_cmp_no_arg(name, key_type, value_type)         \
    value_type *name(const key_type *key, const value_type *base, size_t n,    \
                     int (*cmp)(const key_type *, const value_type *))

#define macro_bsearch_compare_cmp_no_arg(name, key_type, value_type)                  \
    value_type *name(const key_type *key, const value_type *base, size_t n,           \
                     int (*cmp)(const key_type *, const value_type *)) {              \
        __macro_bsearch_code(cmp_no_arg, key_type, value_type, cmp, key, base, n);    \
    }

#define macro_bsearch_compare_h_cmp_arg(name, key_type, value_type)                  \
    value_type *name(const key_type *key, const value_type *base, size_t n,          \
                     int (*cmp)(const key_type *, const value_type *, void *arg),    \
                     void *arg)

#define macro_bsearch_compare_cmp_arg(name, key_type, value_type)                    \
    value_type *name(const key_type *key, const value_type *base, size_t n,          \
                     int (*cmp)(const key_type *, const value_type *, void *arg),    \
                     void *arg) {                                                    \
        __macro_bsearch_code(cmp_arg, key_type, value_type, cmp, key, base, n);      \
    }

#define macro_bsearch_compare_h_arg_cmp(name, key_type, value_type)                  \
    value_type *name(const key_type *key, const value_type *base, size_t n,          \
                     int (*cmp)(void *arg, const key_type *, const value_type *),    \
                     void *arg)

#define macro_bsearch_compare_arg_cmp(name, key_type, value_type)                    \
    value_type *name(const key_type *key, const value_type *base, size_t n,          \
                     int (*cmp)(void *arg, const key_type *, const value_type *),    \
                     void *arg) {                                                    \
        __macro_bsearch_code(arg_cmp, key_type, value_type, cmp, key, base, n);      \
    }

/* end macro_bsearch */
/* start macro_bsearch_lower_bound */

#define macro_bsearch_lower_bound_h_cmp_no_arg(name, key_type, value_type)    \
    value_type *name(const key_type *key, const value_type *base, size_t n)

#define macro_bsearch_lower_bound_cmp_no_arg(name, key_type, value_type, cmp)                     \
    value_type *name(const key_type *key, const value_type *base, size_t n) {                     \
        __macro_bsearch_lower_bound_code(cmp_no_arg, key_type, value_type, cmp, key, base, n);    \
    }

#define __macro_bsearch_lower_bound_h_arg(name, key_type, value_type)    \
    value_type *name(const key_type *key, const value_type *base, size_t n, void *arg)

#define macro_bsearch_lower_bound_h_cmp_arg(name, key_type, value_type, cmp) __macro_bsearch_lower_bound_h_arg(name, key_type, value_type)
#define macro_bsearch_lower_bound_h_arg_cmp(name, key_type, value_type, cmp) __macro_bsearch_lower_bound_h_arg(name, key_type, value_type)

#define __macro_bsearch_lower_bound_arg(name, style, key_type, value_type, cmp)              \
    value_type *name(const key_type *key, const value_type *base, size_t n, void *arg) {     \
        __macro_bsearch_lower_bound_code(style, key_type, value_type, cmp, key, base, n);    \
    }

#define macro_bsearch_lower_bound_cmp_arg(name, key_type, value_type, cmp) __macro_bsearch_lower_bound_arg(name, cmp_arg, key_type, value_type, cmp)
#define macro_bsearch_lower_bound_arg_cmp(name, key_type, value_type, cmp) __macro_bsearch_lower_bound_arg(name, arg_cmp, key_type, value_type, cmp)


#define macro_bsearch_lower_bound_compare_h_cmp_no_arg(name, key_type, value_type)    \
    value_type *name(const key_type *key, const value_type *base, size_t n, int (*cmp)(const key_type *, const value_type *))

#define macro_bsearch_lower_bound_compare_cmp_no_arg(name, key_type, value_type)                                                   \
    value_type *name(const key_type *key, const value_type *base, size_t n, int (*cmp)(const key_type *, const value_type *)) {    \
        __macro_bsearch_lower_bound_code(cmp_no_arg, key_type, value_type, cmp, key, base, n);                                     \
    }

#define macro_bsearch_lower_bound_compare_h_cmp_arg(name, key_type, value_type)      \
    value_type *name(const key_type *key, const value_type *base, size_t n,          \
                     int (*cmp)(const key_type *, const value_type *, void *arg),    \
                     void *arg)

#define macro_bsearch_lower_bound_compare_cmp_arg(name, key_type, value_type)                  \
    value_type *name(const key_type *key, const value_type *base, size_t n,                    \
                     int (*cmp)(const key_type *, const value_type *, void *arg),              \
                     void *arg) {                                                              \
        __macro_bsearch_lower_bound_code(cmp_arg, key_type, value_type, cmp, key, base, n);    \
    }

#define macro_bsearch_lower_bound_compare_h_arg_cmp(name, key_type, value_type)      \
    value_type *name(const key_type *key, const value_type *base, size_t n,          \
                     int (*cmp)(void *arg, const key_type *, const value_type *),    \
                     void *arg)

#define macro_bsearch_lower_bound_compare_arg_cmp(name, key_type, value_type)                  \
    value_type *name(const key_type *key, const value_type *base, size_t n,                    \
                     int (*cmp)(void *arg, const key_type *, const value_type *),              \
                     void *arg) {                                                              \
        __macro_bsearch_lower_bound_code(arg_cmp, key_type, value_type, cmp, key, base, n);    \
    }

/* end macro_bsearch_lower_bound */

/* start macro_bsearch_lower_match */

#define macro_bsearch_lower_match_h_cmp_no_arg(name, key_type, value_type)    \
    value_type *name(const key_type *key, const value_type *base, size_t n)

#define macro_bsearch_lower_match_cmp_no_arg(name, key_type, value_type, cmp)                     \
    value_type *name(const key_type *key, const value_type *base, size_t n) {                     \
        __macro_bsearch_lower_match_code(cmp_no_arg, key_type, value_type, cmp, key, base, n);    \
    }

#define __macro_bsearch_lower_match_h_arg(name, key_type, value_type)    \
    value_type *name(const key_type *key, const value_type *base, size_t n, void *arg)

#define macro_bsearch_lower_match_h_cmp_arg(name, key_type, value_type, cmp) __macro_bsearch_lower_match_h_arg(name, key_type, value_type)
#define macro_bsearch_lower_match_h_arg_cmp(name, key_type, value_type, cmp) __macro_bsearch_lower_match_h_arg(name, key_type, value_type)

#define __macro_bsearch_lower_match_arg(name, style, key_type, value_type, cmp)              \
    value_type *name(const key_type *key, const value_type *base, size_t n, void *arg) {     \
        __macro_bsearch_lower_match_code(style, key_type, value_type, cmp, key, base, n);    \
    }

#define macro_bsearch_lower_match_cmp_arg(name, key_type, value_type, cmp) __macro_bsearch_lower_match_arg(name, cmp_arg, key_type, value_type, cmp)
#define macro_bsearch_lower_match_arg_cmp(name, key_type, value_type, cmp) __macro_bsearch_lower_match_arg(name, arg_cmp, key_type, value_type, cmp)


#define macro_bsearch_lower_match_compare_h_cmp_no_arg(name, key_type, value_type)    \
    value_type *name(const key_type *key, const value_type *base, size_t n, int (*cmp)(const key_type *, const value_type *))

#define macro_bsearch_lower_match_compare_cmp_no_arg(name, key_type, value_type)                                                   \
    value_type *name(const key_type *key, const value_type *base, size_t n, int (*cmp)(const key_type *, const value_type *)) {    \
        __macro_bsearch_lower_match_code(cmp_no_arg, key_type, value_type, cmp, key, base, n);                                     \
    }

#define macro_bsearch_lower_match_compare_h_cmp_arg(name, key_type, value_type)      \
    value_type *name(const key_type *key, const value_type *base, size_t n,          \
                     int (*cmp)(const key_type *, const value_type *, void *arg),    \
                     void *arg)

#define macro_bsearch_lower_match_compare_cmp_arg(name, key_type, value_type)                  \
    value_type *name(const key_type *key, const value_type *base, size_t n,                    \
                     int (*cmp)(const key_type *, const value_type *, void *arg),              \
                     void *arg) {                                                              \
        __macro_bsearch_lower_match_code(cmp_arg, key_type, value_type, cmp, key, base, n);    \
    }

#define macro_bsearch_lower_match_compare_h_arg_cmp(name, key_type, value_type)      \
    value_type *name(const key_type *key, const value_type *base, size_t n,          \
                     int (*cmp)(void *arg, const key_type *, const value_type *),    \
                     void *arg)

#define macro_bsearch_lower_match_compare_arg_cmp(name, key_type, value_type)                  \
    value_type *name(const key_type *key, const value_type *base, size_t n,                    \
                     int (*cmp)(void *arg, const key_type *, const value_type *),              \
                     void *arg) {                                                              \
        __macro_bsearch_lower_match_code(arg_cmp, key_type, value_type, cmp, key, base, n);    \
    }

/* end macro_bsearch_lower_match */

/* start macro_bsearch_upper_bound */

#define macro_bsearch_upper_bound_h_cmp_no_arg(name, key_type, value_type)    \
    value_type *name(const key_type *key, const value_type *base, size_t n)

#define macro_bsearch_upper_bound_cmp_no_arg(name, key_type, value_type, cmp)                     \
    value_type *name(const key_type *key, const value_type *base, size_t n) {                     \
        __macro_bsearch_upper_bound_code(cmp_no_arg, key_type, value_type, cmp, key, base, n);    \
    }

#define __macro_bsearch_upper_bound_h_arg(name, key_type, value_type)    \
    value_type *name(const key_type *key, const value_type *base, size_t n, void *arg)

#define macro_bsearch_upper_bound_h_cmp_arg(name, key_type, value_type, cmp) __macro_bsearch_upper_bound_h_arg(name, key_type, value_type)
#define macro_bsearch_upper_bound_h_arg_cmp(name, key_type, value_type, cmp) __macro_bsearch_upper_bound_h_arg(name, key_type, value_type)

#define __macro_bsearch_upper_bound_arg(name, style, key_type, value_type, cmp)              \
    value_type *name(const key_type *key, const value_type *base, size_t n, void *arg) {     \
        __macro_bsearch_upper_bound_code(style, key_type, value_type, cmp, key, base, n);    \
    }

#define macro_bsearch_upper_bound_cmp_arg(name, key_type, value_type, cmp) __macro_bsearch_upper_bound_arg(name, cmp_arg, key_type, value_type, cmp)
#define macro_bsearch_upper_bound_arg_cmp(name, key_type, value_type, cmp) __macro_bsearch_upper_bound_arg(name, arg_cmp, key_type, value_type, cmp)


#define macro_bsearch_upper_bound_compare_h_cmp_no_arg(name, key_type, value_type)    \
    value_type *name(const key_type *key, const value_type *base, size_t n, int (*cmp)(const key_type *, const value_type *))

#define macro_bsearch_upper_bound_compare_cmp_no_arg(name, key_type, value_type)                                                   \
    value_type *name(const key_type *key, const value_type *base, size_t n, int (*cmp)(const key_type *, const value_type *)) {    \
        __macro_bsearch_upper_bound_code(cmp_no_arg, key_type, value_type, cmp, key, base, n);                                     \
    }

#define macro_bsearch_upper_bound_compare_h_cmp_arg(name, key_type, value_type)      \
    value_type *name(const key_type *key, const value_type *base, size_t n,          \
                     int (*cmp)(const key_type *, const value_type *, void *arg),    \
                     void *arg)

#define macro_bsearch_upper_bound_compare_cmp_arg(name, key_type, value_type)                  \
    value_type *name(const key_type *key, const value_type *base, size_t n,                    \
                     int (*cmp)(const key_type *, const value_type *, void *arg),              \
                     void *arg) {                                                              \
        __macro_bsearch_upper_bound_code(cmp_arg, key_type, value_type, cmp, key, base, n);    \
    }

#define macro_bsearch_upper_bound_compare_h_arg_cmp(name, key_type, value_type)      \
    value_type *name(const key_type *key, const value_type *base, size_t n,          \
                     int (*cmp)(void *arg, const key_type *, const value_type *),    \
                     void *arg)

#define macro_bsearch_upper_bound_compare_arg_cmp(name, key_type, value_type)                  \
    value_type *name(const key_type *key, const value_type *base, size_t n,                    \
                     int (*cmp)(void *arg, const key_type *, const value_type *),              \
                     void *arg) {                                                              \
        __macro_bsearch_upper_bound_code(arg_cmp, key_type, value_type, cmp, key, base, n);    \
    }

/* end macro_bsearch_upper_bound */

/* start macro_bsearch_upper_match */

#define macro_bsearch_upper_match_h_cmp_no_arg(name, key_type, value_type)    \
    value_type *name(const key_type *key, const value_type *base, size_t n)

#define macro_bsearch_upper_match_cmp_no_arg(name, key_type, value_type, cmp)                     \
    value_type *name(const key_type *key, const value_type *base, size_t n) {                     \
        __macro_bsearch_upper_match_code(cmp_no_arg, key_type, value_type, cmp, key, base, n);    \
    }

#define __macro_bsearch_upper_match_h_arg(name, key_type, value_type)    \
    value_type *name(const key_type *key, const value_type *base, size_t n, void *arg)

#define macro_bsearch_upper_match_h_cmp_arg(name, key_type, value_type, cmp) __macro_bsearch_upper_match_h_arg(name, key_type, value_type)
#define macro_bsearch_upper_match_h_arg_cmp(name, key_type, value_type, cmp) __macro_bsearch_upper_match_h_arg(name, key_type, value_type)

#define __macro_bsearch_upper_match_arg(name, style, key_type, value_type, cmp)              \
    value_type *name(const key_type *key, const value_type *base, size_t n, void *arg) {     \
        __macro_bsearch_upper_match_code(style, key_type, value_type, cmp, key, base, n);    \
    }

#define macro_bsearch_upper_match_cmp_arg(name, key_type, value_type, cmp) __macro_bsearch_upper_match_arg(name, cmp_arg, key_type, value_type, cmp)
#define macro_bsearch_upper_match_arg_cmp(name, key_type, value_type, cmp) __macro_bsearch_upper_match_arg(name, arg_cmp, key_type, value_type, cmp)


#define macro_bsearch_upper_match_compare_h_cmp_no_arg(name, key_type, value_type)    \
    value_type *name(const key_type *key, const value_type *base, size_t n, int (*cmp)(const key_type *, const value_type *))

#define macro_bsearch_upper_match_compare_cmp_no_arg(name, key_type, value_type)                                                   \
    value_type *name(const key_type *key, const value_type *base, size_t n, int (*cmp)(const key_type *, const value_type *)) {    \
        __macro_bsearch_upper_match_code(cmp_no_arg, key_type, value_type, cmp, key, base, n);                                     \
    }

#define macro_bsearch_upper_match_compare_h_cmp_arg(name, key_type, value_type)      \
    value_type *name(const key_type *key, const value_type *base, size_t n,          \
                     int (*cmp)(const key_type *, const value_type *, void *arg),    \
                     void *arg)

#define macro_bsearch_upper_match_compare_cmp_arg(name, key_type, value_type)                  \
    value_type *name(const key_type *key, const value_type *base, size_t n,                    \
                     int (*cmp)(const key_type *, const value_type *, void *arg),              \
                     void *arg) {                                                              \
        __macro_bsearch_upper_match_code(cmp_arg, key_type, value_type, cmp, key, base, n);    \
    }

#define macro_bsearch_upper_match_compare_h_arg_cmp(name, key_type, value_type)      \
    value_type *name(const key_type *key, const value_type *base, size_t n,          \
                     int (*cmp)(void *arg, const key_type *, const value_type *),    \
                     void *arg)

#define macro_bsearch_upper_match_compare_arg_cmp(name, key_type, value_type)                  \
    value_type *name(const key_type *key, const value_type *base, size_t n,                    \
                     int (*cmp)(void *arg, const key_type *, const value_type *),              \
                     void *arg) {                                                              \
        __macro_bsearch_upper_match_code(arg_cmp, key_type, value_type, cmp, key, base, n);    \
    }

/* end macro_bsearch_upper_match */
