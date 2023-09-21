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

#ifndef _macro_sort_H
#define _macro_sort_H

#include "the-macro-library/src/macro_introsort.h"

/* See README.md for more details */

/* macro_sort uses macro_introsort */
#define _macro_sort_h(name, style, type) macro_introsort_h(name, style, type)
#define _macro_sort(name, style, type, cmp) macro_introsort(name, style, type, cmp)

#define _macro_sort_compare_h(name, style, type) macro_introsort_compare_h(name, style, type)
#define _macro_sort_compare(name, style, type) macro_introsort_compare(name, style, type)

/* create a set of defaults - less_no_arg */
#define macro_sort_default() less_no_arg

#define macro_sort_h(name, type) _macro_sort_h(name, macro_sort_default(), type)
#define macro_sort(name, type, cmp) _macro_sort(name, macro_sort_default(), type, cmp)

#define macro_sort_compare_h(name, type) _macro_sort_compare_h(name, macro_sort_default(), type)
#define macro_sort_compare(name, type) _macro_sort_compare(name, macro_sort_default(), type)

#endif