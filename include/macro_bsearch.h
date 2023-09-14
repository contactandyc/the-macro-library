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
    See README for more details

    All headers are the same for the different bsearch flavors, simply use macro_bsearch..._h_...
*/

#include "the-macro-library/macro_bsearch_code.h"

#define macro_bsearch_h_cmp_no_arg(name, key_type, value_type)    \
    value_type *name(const key_type *key, const value_type *base, size_t n)

#define __macro_bsearch_h_arg(name, key_type, value_type)    \
    value_type *name(const key_type *key, const value_type *base, size_t n, void *arg)

#define macro_bsearch_h_cmp_arg(name, key_type, value_type, cmp) __macro_bsearch_h_arg(name, key_type, value_type)
#define macro_bsearch_h_arg_cmp(name, key_type, value_type, cmp) __macro_bsearch_h_arg(name, key_type, value_type)

#define macro_bsearch_compare_h_cmp_no_arg(name, key_type, value_type)    \
    value_type *name(const key_type *key, const value_type *base, size_t n, int (*cmp)(const key_type *, const value_type *))

#define macro_bsearch_compare_h_cmp_arg(name, key_type, value_type)                  \
    value_type *name(const key_type *key, const value_type *base, size_t n,          \
                     int (*cmp)(const key_type *, const value_type *, void *arg),    \
                     void *arg)

#define macro_bsearch_compare_h_arg_cmp(name, key_type, value_type)                  \
    value_type *name(const key_type *key, const value_type *base, size_t n,          \
                     int (*cmp)(void *arg, const key_type *, const value_type *),    \
                     void *arg)


#define _macro_bsearch_cmp_no_arg(name, bsearch_style, key_type, value_type, cmp)                           \
    value_type *name(const key_type *key, const value_type *base, size_t n) {                               \
        __macro_bsearch_ ## bsearch_style ## _code(cmp_no_arg, key_type, value_type, cmp, key, base, n);    \
    }

#define __macro_bsearch_arg(name, bsearch_style, style, key_type, value_type, cmp)                     \
    value_type *name(const key_type *key, const value_type *base, size_t n, void *arg) {               \
        __macro_bsearch_ ## bsearch_style ## _code(style, key_type, value_type, cmp, key, base, n);    \
    }

#define _macro_bsearch_cmp_arg(name, bsearch_style, key_type, value_type, cmp) __macro_bsearch_arg(name, bsearch_style, cmp_arg, key_type, value_type, cmp)
#define _macro_bsearch_arg_cmp(name, bsearch_style, key_type, value_type, cmp) __macro_bsearch_arg(name, bsearch_style, arg_cmp, key_type, value_type, cmp)

#define _macro_bsearch_compare_cmp_no_arg(name, bsearch_style, key_type, value_type)                                               \
    value_type *name(const key_type *key, const value_type *base, size_t n, int (*cmp)(const key_type *, const value_type *)) {    \
        __macro_bsearch_ ## bsearch_style ## _code(cmp_no_arg, key_type, value_type, cmp, key, base, n);                           \
    }

#define _macro_bsearch_compare_cmp_arg(name, bsearch_style, key_type, value_type)                        \
    value_type *name(const key_type *key, const value_type *base, size_t n,                              \
                     int (*cmp)(const key_type *, const value_type *, void *arg),                        \
                     void *arg) {                                                                        \
        __macro_bsearch_ ## bsearch_style ## _code(cmp_arg, key_type, value_type, cmp, key, base, n);    \
    }

#define _macro_bsearch_compare_arg_cmp(name, bsearch_style, key_type, value_type)                        \
    value_type *name(const key_type *key, const value_type *base, size_t n,                              \
                     int (*cmp)(void *arg, const key_type *, const value_type *),                        \
                     void *arg) {                                                                        \
        __macro_bsearch_ ## bsearch_style ## _code(arg_cmp, key_type, value_type, cmp, key, base, n);    \
    }


#define macro_bsearch_cmp_no_arg(name, key_type, value_type, cmp) _macro_bsearch_cmp_no_arg(name, core, key_type, value_type, cmp)
#define macro_bsearch_cmp_arg(name, key_type, value_type, cmp) _macro_bsearch_cmp_arg(name, core, key_type, value_type, cmp)
#define macro_bsearch_arg_cmp(name, key_type, value_type, cmp) _macro_bsearch_arg_cmp(name, core, key_type, value_type, cmp)
#define macro_bsearch_compare_cmp_no_arg(name, key_type, value_type, cmp) _macro_bsearch_compare_cmp_no_arg(name, core, key_type, value_type, cmp)
#define macro_bsearch_compare_cmp_arg(name, key_type, value_type, cmp) _macro_bsearch_compare_cmp_arg(name, core, key_type, value_type, cmp)
#define macro_bsearch_compare_arg_cmp(name, key_type, value_type, cmp) _macro_bsearch_compare_arg_cmp(name, core, key_type, value_type, cmp)

#define macro_bsearch_first_cmp_no_arg(name, key_type, value_type, cmp) _macro_bsearch_cmp_no_arg(name, first, key_type, value_type, cmp)
#define macro_bsearch_first_cmp_arg(name, key_type, value_type, cmp) _macro_bsearch_cmp_arg(name, first, key_type, value_type, cmp)
#define macro_bsearch_first_arg_cmp(name, key_type, value_type, cmp) _macro_bsearch_arg_cmp(name, first, key_type, value_type, cmp)
#define macro_bsearch_first_compare_cmp_no_arg(name, key_type, value_type, cmp) _macro_bsearch_compare_cmp_no_arg(name, first, key_type, value_type, cmp)
#define macro_bsearch_first_compare_cmp_arg(name, key_type, value_type, cmp) _macro_bsearch_compare_cmp_arg(name, first, key_type, value_type, cmp)
#define macro_bsearch_first_compare_arg_cmp(name, key_type, value_type, cmp) _macro_bsearch_compare_arg_cmp(name, first, key_type, value_type, cmp)

#define macro_bsearch_last_cmp_no_arg(name, key_type, value_type, cmp) _macro_bsearch_cmp_no_arg(name, last, key_type, value_type, cmp)
#define macro_bsearch_last_cmp_arg(name, key_type, value_type, cmp) _macro_bsearch_cmp_arg(name, last, key_type, value_type, cmp)
#define macro_bsearch_last_arg_cmp(name, key_type, value_type, cmp) _macro_bsearch_arg_cmp(name, last, key_type, value_type, cmp)
#define macro_bsearch_last_compare_cmp_no_arg(name, key_type, value_type, cmp) _macro_bsearch_compare_cmp_no_arg(name, last, key_type, value_type, cmp)
#define macro_bsearch_last_compare_cmp_arg(name, key_type, value_type, cmp) _macro_bsearch_compare_cmp_arg(name, last, key_type, value_type, cmp)
#define macro_bsearch_last_compare_arg_cmp(name, key_type, value_type, cmp) _macro_bsearch_compare_arg_cmp(name, last, key_type, value_type, cmp)

#define macro_bsearch_floor_cmp_no_arg(name, key_type, value_type, cmp) _macro_bsearch_cmp_no_arg(name, floor, key_type, value_type, cmp)
#define macro_bsearch_floor_cmp_arg(name, key_type, value_type, cmp) _macro_bsearch_cmp_arg(name, floor, key_type, value_type, cmp)
#define macro_bsearch_floor_arg_cmp(name, key_type, value_type, cmp) _macro_bsearch_arg_cmp(name, floor, key_type, value_type, cmp)
#define macro_bsearch_floor_compare_cmp_no_arg(name, key_type, value_type, cmp) _macro_bsearch_compare_cmp_no_arg(name, floor, key_type, value_type, cmp)
#define macro_bsearch_floor_compare_cmp_arg(name, key_type, value_type, cmp) _macro_bsearch_compare_cmp_arg(name, floor, key_type, value_type, cmp)
#define macro_bsearch_floor_compare_arg_cmp(name, key_type, value_type, cmp) _macro_bsearch_compare_arg_cmp(name, floor, key_type, value_type, cmp)

#define macro_bsearch_ceiling_cmp_no_arg(name, key_type, value_type, cmp) _macro_bsearch_cmp_no_arg(name, ceiling, key_type, value_type, cmp)
#define macro_bsearch_ceiling_cmp_arg(name, key_type, value_type, cmp) _macro_bsearch_cmp_arg(name, ceiling, key_type, value_type, cmp)
#define macro_bsearch_ceiling_arg_cmp(name, key_type, value_type, cmp) _macro_bsearch_arg_cmp(name, ceiling, key_type, value_type, cmp)
#define macro_bsearch_ceiling_compare_cmp_no_arg(name, key_type, value_type, cmp) _macro_bsearch_compare_cmp_no_arg(name, ceiling, key_type, value_type, cmp)
#define macro_bsearch_ceiling_compare_cmp_arg(name, key_type, value_type, cmp) _macro_bsearch_compare_cmp_arg(name, ceiling, key_type, value_type, cmp)
#define macro_bsearch_ceiling_compare_arg_cmp(name, key_type, value_type, cmp) _macro_bsearch_compare_arg_cmp(name, ceiling, key_type, value_type, cmp)

#define macro_bsearch_lower_bound_cmp_no_arg(name, key_type, value_type, cmp) _macro_bsearch_cmp_no_arg(name, lower_bound, key_type, value_type, cmp)
#define macro_bsearch_lower_bound_cmp_arg(name, key_type, value_type, cmp) _macro_bsearch_cmp_arg(name, lower_bound, key_type, value_type, cmp)
#define macro_bsearch_lower_bound_arg_cmp(name, key_type, value_type, cmp) _macro_bsearch_arg_cmp(name, lower_bound, key_type, value_type, cmp)
#define macro_bsearch_lower_bound_compare_cmp_no_arg(name, key_type, value_type, cmp) _macro_bsearch_compare_cmp_no_arg(name, lower_bound, key_type, value_type, cmp)
#define macro_bsearch_lower_bound_compare_cmp_arg(name, key_type, value_type, cmp) _macro_bsearch_compare_cmp_arg(name, lower_bound, key_type, value_type, cmp)
#define macro_bsearch_lower_bound_compare_arg_cmp(name, key_type, value_type, cmp) _macro_bsearch_compare_arg_cmp(name, lower_bound, key_type, value_type, cmp)

#define macro_bsearch_upper_bound_cmp_no_arg(name, key_type, value_type, cmp) _macro_bsearch_cmp_no_arg(name, upper_bound, key_type, value_type, cmp)
#define macro_bsearch_upper_bound_cmp_arg(name, key_type, value_type, cmp) _macro_bsearch_cmp_arg(name, upper_bound, key_type, value_type, cmp)
#define macro_bsearch_upper_bound_arg_cmp(name, key_type, value_type, cmp) _macro_bsearch_arg_cmp(name, upper_bound, key_type, value_type, cmp)
#define macro_bsearch_upper_bound_compare_cmp_no_arg(name, key_type, value_type, cmp) _macro_bsearch_compare_cmp_no_arg(name, upper_bound, key_type, value_type, cmp)
#define macro_bsearch_upper_bound_compare_cmp_arg(name, key_type, value_type, cmp) _macro_bsearch_compare_cmp_arg(name, upper_bound, key_type, value_type, cmp)
#define macro_bsearch_upper_bound_compare_arg_cmp(name, key_type, value_type, cmp) _macro_bsearch_compare_arg_cmp(name, upper_bound, key_type, value_type, cmp)
