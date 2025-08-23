// SPDX-FileCopyrightText: 2019–2025 Andy Curtis <contactandyc@gmail.com>
// SPDX-FileCopyrightText: 2024–2025 Knode.ai — technical questions: contact Andy (above)
// SPDX-License-Identifier: Apache-2.0

/*
    See README for more details

    All headers are the same for the different bsearch flavors, simply use macro_bsearch..._h_...
*/
#ifndef _macro_bsearch_H
#define _macro_bsearch_H

#include "the-macro-library/src/macro_bsearch_code.h"

#define _macro_bsearch_h(name, style, value_type)                      \
    value_type *name(const value_type *key, const value_type *base,    \
                     macro_cmp_signature(size_t n, style, value_type))

#define _macro_bsearch(name, bsearch_style, style, value_type, cmp)                          \
    _macro_bsearch_h(name, style, value_type) {                                              \
        __macro_bsearch_ ## bsearch_style ## _code(style, value_type, cmp, key, base, n);    \
    }

#define _macro_bsearch_kv_h(name, style, key_type, value_type)       \
    value_type *name(const key_type *key, const value_type *base,    \
                     macro_cmp_kv_signature(size_t n, style, key_type, value_type))

#define _macro_bsearch_kv(name, bsearch_style, style, key_type, value_type, cmp)                          \
    _macro_bsearch_kv_h(name, style, key_type, value_type) {                                              \
        __macro_bsearch_kv_ ## bsearch_style ## _code(style, key_type, value_type, cmp, key, base, n);    \
    }

#define _macro_bsearch_compare_h(name, style, value_type)              \
    value_type *name(const value_type *key, const value_type *base,    \
                     macro_compare_signature(size_t n, style, value_type))

#define _macro_bsearch_compare(name, bsearch_style, style, value_type)                       \
    _macro_bsearch_compare_h(name, style, value_type) {                                      \
        __macro_bsearch_ ## bsearch_style ## _code(style, value_type, cmp, key, base, n);    \
    }

#define _macro_bsearch_kv_compare_h(name, style, key_type, value_type)    \
    value_type *name(const key_type *key, const value_type *base,         \
                     macro_compare_kv_signature(size_t n, style, key_type, value_type))

#define _macro_bsearch_kv_compare(name, bsearch_style, style, key_type, value_type)                       \
    _macro_bsearch_kv_compare_h(name, style, key_type, value_type) {                                      \
        __macro_bsearch_kv_ ## bsearch_style ## _code(style, key_type, value_type, cmp, key, base, n);    \
    }

#define macro_bsearch_default() cmp_no_arg

#define macro_bsearch_h(name, value_type)    \
    _macro_bsearch_h(name, macro_bsearch_default(), value_type )

#define macro_bsearch_compare_h(name, value_type)    \
    _macro_bsearch_compare_h(name, macro_bsearch_default(), value_type )

#define macro_bsearch(name, value_type, cmp)    \
    _macro_bsearch(name, core, macro_bsearch_default(), value_type, cmp)

#define macro_bsearch_compare(name, value_type)    \
    _macro_bsearch_compare(name, core, macro_bsearch_default(), value_type )

#define macro_bsearch_first(name, value_type, cmp)    \
    _macro_bsearch(name, first, macro_bsearch_default(), value_type, cmp)

#define macro_bsearch_first_compare(name, value_type)    \
    _macro_bsearch_compare(name, first, macro_bsearch_default(), value_type )

#define macro_bsearch_last(name, value_type, cmp)    \
    _macro_bsearch(name, last, macro_bsearch_default(), value_type, cmp)

#define macro_bsearch_last_compare(name, value_type)    \
    _macro_bsearch_compare(name, last, macro_bsearch_default(), value_type )

#define macro_bsearch_floor(name, value_type, cmp)    \
    _macro_bsearch(name, floor, macro_bsearch_default(), value_type, cmp)

#define macro_bsearch_floor_compare(name, value_type)    \
    _macro_bsearch_compare(name, floor, macro_bsearch_default(), value_type )

#define macro_bsearch_ceiling(name, value_type, cmp)    \
    _macro_bsearch(name, ceiling, macro_bsearch_default(), value_type, cmp)

#define macro_bsearch_ceiling_compare(name, value_type)    \
    _macro_bsearch_compare(name, ceiling, macro_bsearch_default(), value_type)

#define macro_bsearch_lower_bound(name, value_type, cmp)    \
    _macro_bsearch(name, lower_bound, macro_bsearch_default(), value_type, cmp)

#define macro_bsearch_lower_bound_compare(name, value_type)    \
    _macro_bsearch_compare(name, lower_bound, macro_bsearch_default(), value_type )

#define macro_bsearch_upper_bound(name, value_type, cmp)    \
    _macro_bsearch(name, upper_bound, macro_bsearch_default(), value_type, cmp)

#define macro_bsearch_upper_bound_compare(name, value_type)    \
    _macro_bsearch_compare(name, upper_bound, macro_bsearch_default(), value_type )

/* kv version (accepts a different type of key) */

#define macro_bsearch_kv_h(name, key_type, value_type)    \
    _macro_bsearch_kv_h(name, macro_bsearch_default(), key_type, value_type )

#define macro_bsearch_kv_compare_h(name, key_type, value_type)    \
    _macro_bsearch_kv_compare_h(name, macro_bsearch_default(), key_type, value_type )

#define macro_bsearch_kv(name, key_type, value_type, cmp)    \
    _macro_bsearch_kv(name, core, macro_bsearch_default(), key_type, value_type, cmp)

#define macro_bsearch_kv_compare(name, key_type, value_type)    \
    _macro_bsearch_kv_compare(name, core, macro_bsearch_default(), key_type, value_type )

#define macro_bsearch_first_kv(name, key_type, value_type, cmp)    \
    _macro_bsearch_kv(name, first, macro_bsearch_default(), key_type, value_type, cmp)

#define macro_bsearch_first_kv_compare(name, key_type, value_type)    \
    _macro_bsearch_kv_compare(name, first, macro_bsearch_default(), key_type, value_type )

#define macro_bsearch_last_kv(name, key_type, value_type, cmp)    \
    _macro_bsearch_kv(name, last, macro_bsearch_default(), key_type, value_type, cmp)

#define macro_bsearch_last_kv_compare(name, key_type, value_type)    \
    _macro_bsearch_kv_compare(name, last, macro_bsearch_default(), key_type, value_type )

#define macro_bsearch_floor_kv(name, key_type, value_type, cmp)    \
    _macro_bsearch_kv(name, floor, macro_bsearch_default(), key_type, value_type, cmp)

#define macro_bsearch_floor_kv_compare(name, key_type, value_type)    \
    _macro_bsearch_kv_compare(name, floor, macro_bsearch_default(), key_type, value_type )

#define macro_bsearch_ceiling_kv(name, key_type, value_type, cmp)    \
    _macro_bsearch_kv(name, ceiling, macro_bsearch_default(), key_type, value_type, cmp)

#define macro_bsearch_ceiling_kv_compare(name, key_type, value_type)    \
    _macro_bsearch_kv_compare(name, ceiling, macro_bsearch_default(), key_type, value_type )

#define macro_bsearch_lower_bound_kv(name, key_type, value_type, cmp)    \
    _macro_bsearch_kv(name, lower_bound, macro_bsearch_default(), key_type, value_type, cmp)

#define macro_bsearch_lower_bound_kv_compare(name, key_type, value_type)    \
    _macro_bsearch_kv_compare(name, lower_bound, macro_bsearch_default(), key_type, value_type )

#define macro_bsearch_upper_bound_kv(name, key_type, value_type, cmp)    \
    _macro_bsearch_kv(name, upper_bound, macro_bsearch_default(), key_type, value_type, cmp)

#define macro_bsearch_upper_bound_kv_compare(name, key_type, value_type)    \
    _macro_bsearch_kv_compare(name, upper_bound, macro_bsearch_default(), key_type, value_type )

#endif
