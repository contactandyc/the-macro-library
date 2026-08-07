// SPDX-FileCopyrightText: 2019–2026 Andy Curtis <contactandyc@gmail.com>
// SPDX-FileCopyrightText: 2024–2025 Knode.ai
// SPDX-License-Identifier: Apache-2.0
//
// Maintainer: Andy Curtis <contactandyc@gmail.com>

#ifndef _macro_heap_H
#define _macro_heap_H

#include "the-macro-library/src/macro_heap_sort.h"

#define macro_heap_t(name, type)    \
    typedef struct {                \
         type* arr;                 \
         size_t num;                \
         size_t size;               \
    } name

/* The heap_push functions expect the item to be put just after the array.
   After calling heap_push, the array will have one more element.  heap_push
   does not check memory size as the element to be pushed is already past
   the end of the valid heap (so it must be checked beforehand if relevant).

   The heap_pop returns the 0th element after moving it just after the array.

   heap_replace expects the caller to replace the 0th element which is popped and
   then pushed to move it into the right location.  The replaced element is tested
   to see if it is in the right position prior to popping and pushing.
*/
#define macro_heap_replace_h(name, style, type, heap_type)    \
void name(macro_cmp_signature(heap_type* ht, style, type))

#define _macro_heap_replace(name, style, type, cmp, heap_type, heap_style)            \
macro_heap_replace_h(name, style, type, heap_type) {                                  \
    macro_check_ ## heap_style ## _heap_zero(style, type, cmp, ht->arr, ht->num);     \
    __macro_ ## heap_style ## _heap_pop_code(style, type, cmp, ht->arr, ht->num);     \
    __macro_ ## heap_style ## _heap_push_code(style, type, cmp, ht->arr, ht->num);    \
}

#define macro_min_heap_replace(name, style, type, cmp, heap_type)    \
    _macro_heap_replace(name, style, type, cmp, heap_type, min)

#define macro_max_heap_replace(name, style, type, cmp, heap_type)    \
    _macro_heap_replace(name, style, type, cmp, heap_type, max)

#define macro_heap_replace_compare_h(name, style, type, heap_type)    \
void name(macro_cmp_signature(heap_type* ht, compare_ ## style, type))

#define _macro_heap_replace_compare(name, style, type, heap_type, heap_style)         \
macro_heap_replace_compare_h(name, style, type, heap_type) {                          \
    macro_check_ ## heap_style ## _heap_zero(style, type, cmp, ht->arr, ht->num);     \
    __macro_ ## heap_style ## _heap_pop_code(style, type, cmp, ht->arr, ht->num);     \
    __macro_ ## heap_style ## _heap_push_code(style, type, cmp, ht->arr, ht->num);    \
}

#define macro_min_heap_replace_compare(name, style, type, heap_type)    \
    _macro_heap_replace_compare(name, style, type, heap_type, min)

#define macro_max_heap_replace_compare(name, style, type, heap_type)    \
    _macro_heap_replace_compare(name, style, type, heap_type, max)

#define macro_heap_push_h(name, style, type, heap_type)    \
void name(macro_cmp_signature(heap_type* ht, style, type))

#define _macro_heap_push(name, style, type, cmp, heap_type, heap_style)               \
macro_heap_push_h(name, style, type, heap_type) {                                     \
    type tmp;                                                                         \
    __macro_ ## heap_style ## _heap_push_code(style, type, cmp, ht->arr, ht->num);    \
}

#define macro_min_heap_push(name, style, type, cmp, heap_type)    \
    _macro_heap_push(name, style, type, cmp, heap_type, min)

#define macro_max_heap_push(name, style, type, cmp, heap_type)    \
    _macro_heap_push(name, style, type, cmp, heap_type, max)

#define macro_heap_push_compare_h(name, style, type, heap_type)    \
void name(macro_cmp_signature(heap_type* ht, compare_ ## style, type))

#define _macro_heap_push_compare(name, style, type, heap_type, heap_style)            \
macro_heap_push_compare_h(name, style, type, heap_type) {                             \
    type tmp;                                                                         \
    __macro_ ## heap_style ## _heap_push_code(style, type, cmp, ht->arr, ht->num);    \
}

#define macro_min_heap_push_compare(name, style, type, heap_type)    \
    _macro_heap_push_compare(name, style, type, heap_type, min)

#define macro_max_heap_push_compare(name, style, type, heap_type )    \
    _macro_heap_push_compare(name, style, type, heap_type, max)


#define macro_heap_pop_h(name, style, type, heap_type)    \
type* name(macro_cmp_signature(heap_type* ht, style, type))

#define _macro_heap_pop(name, style, type, cmp, heap_type, heap_style)               \
macro_heap_pop_h(name, style, type, heap_type) {                                     \
    if(!ht->num) return NULL;                                                        \
    __macro_ ## heap_style ## _heap_pop_code(style, type, cmp, ht->arr, ht->num);    \
    return ht->arr + ht->num;                                                        \
}

#define macro_min_heap_pop(name, style, type, cmp, heap_type)    \
    _macro_heap_pop(name, style, type, cmp, heap_type, min)

#define macro_max_heap_pop(name, style, type, cmp, heap_type)    \
    _macro_heap_pop(name, style, type, cmp, heap_type, max)

#define macro_heap_pop_compare_h(name, style, type, heap_type)    \
type* name(macro_cmp_signature(heap_type* ht, compare_ ## style, type))

#define _macro_heap_pop_compare(name, style, type, heap_type, heap_style)            \
macro_heap_pop_compare_h(name, style, type, heap_type) {                             \
    if(!ht->num) return NULL;                                                        \
    __macro_ ## heap_style ## _heap_pop_code(style, type, cmp, ht->arr, ht->num);    \
    return ht->arr + ht->num;                                                        \
}

#define macro_min_heap_pop_compare(name, style, type, heap_type)    \
    _macro_heap_pop_compare(name, style, type, heap_type, min)

#define macro_max_heap_pop_compare(name, style, type, heap_type)    \
    _macro_heap_pop_compare(name, style, type, heap_type, max)

/* ==========================================================================
 * INTERNAL GENERATOR ENGINE (Linkage Parameterized)
 * ========================================================================== */

#define macro_heap_default() less_no_arg

/* Min Heap */
#define __macro_min_heap_api_h(MOD, prefix, type, heap_type) \
    MOD macro_heap_push_h(prefix##_push, macro_heap_default(), type, heap_type); \
    MOD macro_heap_pop_h(prefix##_pop, macro_heap_default(), type, heap_type); \
    MOD macro_heap_replace_h(prefix##_replace, macro_heap_default(), type, heap_type)

#define __macro_min_heap_api(MOD, prefix, type, cmp, heap_type) \
    MOD _macro_heap_push(prefix##_push, macro_heap_default(), type, cmp, heap_type, min) \
    MOD _macro_heap_pop(prefix##_pop, macro_heap_default(), type, cmp, heap_type, min) \
    MOD _macro_heap_replace(prefix##_replace, macro_heap_default(), type, cmp, heap_type, min)

/* Max Heap */
#define __macro_max_heap_api_h(MOD, prefix, type, heap_type) \
    MOD macro_heap_push_h(prefix##_push, macro_heap_default(), type, heap_type); \
    MOD macro_heap_pop_h(prefix##_pop, macro_heap_default(), type, heap_type); \
    MOD macro_heap_replace_h(prefix##_replace, macro_heap_default(), type, heap_type)

#define __macro_max_heap_api(MOD, prefix, type, cmp, heap_type) \
    MOD _macro_heap_push(prefix##_push, macro_heap_default(), type, cmp, heap_type, max) \
    MOD _macro_heap_pop(prefix##_pop, macro_heap_default(), type, cmp, heap_type, max) \
    MOD _macro_heap_replace(prefix##_replace, macro_heap_default(), type, cmp, heap_type, max)

/* Min Heap Compare */
#define __macro_min_heap_compare_api_h(MOD, prefix, type, heap_type) \
    MOD macro_heap_push_compare_h(prefix##_push, macro_heap_default(), type, heap_type); \
    MOD macro_heap_pop_compare_h(prefix##_pop, macro_heap_default(), type, heap_type); \
    MOD macro_heap_replace_compare_h(prefix##_replace, macro_heap_default(), type, heap_type)

#define __macro_min_heap_compare_api(MOD, prefix, type, heap_type) \
    MOD _macro_heap_push_compare(prefix##_push, macro_heap_default(), type, heap_type, min) \
    MOD _macro_heap_pop_compare(prefix##_pop, macro_heap_default(), type, heap_type, min) \
    MOD _macro_heap_replace_compare(prefix##_replace, macro_heap_default(), type, heap_type, min)

/* Max Heap Compare */
#define __macro_max_heap_compare_api_h(MOD, prefix, type, heap_type) \
    MOD macro_heap_push_compare_h(prefix##_push, macro_heap_default(), type, heap_type); \
    MOD macro_heap_pop_compare_h(prefix##_pop, macro_heap_default(), type, heap_type); \
    MOD macro_heap_replace_compare_h(prefix##_replace, macro_heap_default(), type, heap_type)

#define __macro_max_heap_compare_api(MOD, prefix, type, heap_type) \
    MOD _macro_heap_push_compare(prefix##_push, macro_heap_default(), type, heap_type, max) \
    MOD _macro_heap_pop_compare(prefix##_pop, macro_heap_default(), type, heap_type, max) \
    MOD _macro_heap_replace_compare(prefix##_replace, macro_heap_default(), type, heap_type, max)

/* ==========================================================================
 * PUBLIC GENERATORS
 * ========================================================================== */

/* Min Heap APIs */
#define macro_min_heap_api_h(prefix, type, heap_type)                  __macro_min_heap_api_h(, prefix, type, heap_type)
#define macro_min_heap_static_api_h(prefix, type, heap_type)           __macro_min_heap_api_h(static, prefix, type, heap_type)
#define macro_min_heap_static_inline_api_h(prefix, type, heap_type)    __macro_min_heap_api_h(static inline, prefix, type, heap_type)

#define macro_min_heap_api(prefix, type, cmp, heap_type)               __macro_min_heap_api(, prefix, type, cmp, heap_type)
#define macro_min_heap_static_api(prefix, type, cmp, heap_type)        __macro_min_heap_api(static, prefix, type, cmp, heap_type)
#define macro_min_heap_static_inline_api(prefix, type, cmp, heap_type) __macro_min_heap_api(static inline, prefix, type, cmp, heap_type)

/* Max Heap APIs */
#define macro_max_heap_api_h(prefix, type, heap_type)                  __macro_max_heap_api_h(, prefix, type, heap_type)
#define macro_max_heap_static_api_h(prefix, type, heap_type)           __macro_max_heap_api_h(static, prefix, type, heap_type)
#define macro_max_heap_static_inline_api_h(prefix, type, heap_type)    __macro_max_heap_api_h(static inline, prefix, type, heap_type)

#define macro_max_heap_api(prefix, type, cmp, heap_type)               __macro_max_heap_api(, prefix, type, cmp, heap_type)
#define macro_max_heap_static_api(prefix, type, cmp, heap_type)        __macro_max_heap_api(static, prefix, type, cmp, heap_type)
#define macro_max_heap_static_inline_api(prefix, type, cmp, heap_type) __macro_max_heap_api(static inline, prefix, type, cmp, heap_type)

/* Min Heap Compare APIs */
#define macro_min_heap_compare_api_h(prefix, type, heap_type)                  __macro_min_heap_compare_api_h(, prefix, type, heap_type)
#define macro_min_heap_static_compare_api_h(prefix, type, heap_type)           __macro_min_heap_compare_api_h(static, prefix, type, heap_type)
#define macro_min_heap_static_inline_compare_api_h(prefix, type, heap_type)    __macro_min_heap_compare_api_h(static inline, prefix, type, heap_type)

#define macro_min_heap_compare_api(prefix, type, heap_type)               __macro_min_heap_compare_api(, prefix, type, heap_type)
#define macro_min_heap_static_compare_api(prefix, type, heap_type)        __macro_min_heap_compare_api(static, prefix, type, heap_type)
#define macro_min_heap_static_inline_compare_api(prefix, type, heap_type) __macro_min_heap_compare_api(static inline, prefix, type, heap_type)

/* Max Heap Compare APIs */
#define macro_max_heap_compare_api_h(prefix, type, heap_type)                  __macro_max_heap_compare_api_h(, prefix, type, heap_type)
#define macro_max_heap_static_compare_api_h(prefix, type, heap_type)           __macro_max_heap_compare_api_h(static, prefix, type, heap_type)
#define macro_max_heap_static_inline_compare_api_h(prefix, type, heap_type)    __macro_max_heap_compare_api_h(static inline, prefix, type, heap_type)

#define macro_max_heap_compare_api(prefix, type, heap_type)               __macro_max_heap_compare_api(, prefix, type, heap_type)
#define macro_max_heap_static_compare_api(prefix, type, heap_type)        __macro_max_heap_compare_api(static, prefix, type, heap_type)
#define macro_max_heap_static_inline_compare_api(prefix, type, heap_type) __macro_max_heap_compare_api(static inline, prefix, type, heap_type)

#endif
