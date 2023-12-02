/*
Copyright (c) 2023 Andy Curtis

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

#ifndef _macro_heap_H
#define _macro_heap_H

#include "the-macro-library/src/macro_heap_sort.h"

#define macro_heap_t(name, type)    \
    typedef struct {           \
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
#define macro_heap_replace_h(name, style, type, heap_type)                  \
void name(macro_cmp_signature(heap_type* ht, style, type))

#define _macro_heap_replace(name, style, type, cmp, heap_type, heap_style)          \
macro_heap_replace_h(name, style, type, heap_type) {                                \
    macro_check_ ## heap_style ## _heap_zero(style, type, cmp, ht->arr, ht->num);         \
    __macro_ ## heap_style ## _heap_pop_code(style, type, cmp, ht->arr, ht->num);   \
    __macro_ ## heap_style ## _heap_push_code(style, type, cmp, ht->arr, ht->num);  \
}

#define macro_min_heap_replace(name, style, type, cmp, heap_type)  \
    _macro_heap_replace(name, style, type, cmp, heap_type, min)

#define macro_max_heap_replace(name, style, type, cmp, heap_type)  \
    _macro_heap_replace(name, style, type, cmp, heap_type, max)

#define macro_heap_replace_compare_h(name, style, type, heap_type)             \
void name(macro_cmp_signature(heap_type* ht, compare_ ## style, type))

#define _macro_heap_replace_compare(name, style, type, heap_type, heap_style)    \
macro_heap_replace_compare_h(name, style, type, heap_type) {                         \
    macro_check_ ## heap_style ## _heap_zero(style, type, cmp, ht->arr, ht->num);          \
    __macro_ ## heap_style ## _heap_pop_code(style, type, cmp, ht->arr, ht->num);    \
    __macro_ ## heap_style ## _heap_push_code(style, type, cmp, ht->arr, ht->num);   \
}

#define macro_min_heap_replace_compare(name, style, type, heap_type)  \
    _macro_heap_replace_compare(name, style, type, heap_type, min)

#define macro_max_heap_replace_compare(name, style, type, heap_type)  \
    _macro_heap_replace_compare(name, style, type, heap_type, max)

#define macro_heap_push_h(name, style, type, heap_type)                       \
void name(macro_cmp_signature(heap_type* ht, style, type))

#define _macro_heap_push(name, style, type, cmp, heap_type, heap_style)             \
macro_heap_push_h(name, style, type, heap_type) {                                   \
    type tmp;                                                                      \
    __macro_ ## heap_style ## _heap_push_code(style, type, cmp, ht->arr, ht->num);  \
}

#define macro_min_heap_push(name, style, type, cmp, heap_type)  \
    _macro_heap_push(name, style, type, cmp, heap_type, min)

#define macro_max_heap_push(name, style, type, cmp, heap_type)  \
    _macro_heap_push(name, style, type, cmp, heap_type, max)

#define macro_heap_push_compare_h(name, style, type, heap_type)                \
void name(macro_cmp_signature(heap_type* ht, compare_ ## style, type))

#define _macro_heap_push_compare(name, style, type, heap_type, heap_style)             \
macro_heap_push_compare_h(name, style, type, heap_type) {                              \
    type tmp;                                                                      \
    __macro_ ## heap_style ## _heap_push_code(style, type, cmp, ht->arr, ht->num);     \
}

#define macro_min_heap_push_compare(name, style, type, heap_type)  \
    _macro_heap_push_compare(name, style, type, heap_type, min)

#define macro_max_heap_push_compare(name, style, type, heap_type )  \
    _macro_heap_push_compare(name, style, type, heap_type, max)


#define macro_heap_pop_h(name, style, type, heap_type)                        \
type* name(macro_cmp_signature(heap_type* ht, style, type))

#define _macro_heap_pop(name, style, type, cmp, heap_type, heap_style)             \
macro_heap_pop_h(name, style, type, heap_type) {                                   \
    if(!ht->num) return NULL;                                                      \
    __macro_ ## heap_style ## _heap_pop_code(style, type, cmp, ht->arr, ht->num);  \
    return ht->arr + ht->num;                                                      \
}

#define macro_min_heap_pop(name, style, type, cmp, heap_type)  \
    _macro_heap_pop(name, style, type, cmp, heap_type, min)

#define macro_max_heap_pop(name, style, type, cmp, heap_type)  \
    _macro_heap_pop(name, style, type, cmp, heap_type, max)

#define macro_heap_pop_compare_h(name, style, type, heap_type)               \
type* name(macro_cmp_signature(heap_type* ht, compare_ ## style, type))

#define _macro_heap_pop_compare(name, style, type, heap_type, heap_style)           \
macro_heap_pop_compare_h(name, style, type, heap_type) {                            \
    if(!ht->num) return NULL;                                                       \
    __macro_ ## heap_style ## _heap_pop_code(style, type, cmp, ht->arr, ht->num);   \
    return ht->arr + ht->num;                                                       \
}

#define macro_min_heap_pop_compare(name, style, type, heap_type)  \
    _macro_heap_pop_compare(name, style, type, heap_type, min)

#define macro_max_heap_pop_compare(name, style, type, heap_type)  \
    _macro_heap_pop_compare(name, style, type, heap_type, max)

#endif