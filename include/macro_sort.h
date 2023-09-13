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

#ifndef _macro_sort_H
#define _macro_sort_H

#include "the-macro-library/macro_introsort.h"

/* See README.md for more details */

#define macro_sort_h_cmp_no_arg(name, type) macro_introsort1_h(name, type)
#define macro_sort_h_cmp_arg(name, type) macro_introsort2_h(name, type)
#define macro_sort_h_arg_cmp(name, type) macro_introsort2_h(name, type)
#define macro_sort_h_less_no_arg(name, type) macro_introsort1_h(name, type)
#define macro_sort_h_less_arg(name, type) macro_introsort2_h(name, type)
#define macro_sort_h_arg_less(name, type) macro_introsort2_h(name, type)
#define macro_sort_h_less(name, type) macro_introsort1_h(name, type)
#define macro_sort_h_cmp(name, type) macro_introsort1_h(name, type)

#define macro_sort_cmp_no_arg(name, type, cmp) macro_introsort1(name, cmp_no_arg, type, cmp)
#define macro_sort_cmp_arg(name, type, cmp) macro_introsort2(name, cmp_arg, type, cmp)
#define macro_sort_arg_cmp(name, type, cmp) macro_introsort2(name, arg_cmp, type, cmp)
#define macro_sort_less_no_arg(name, type, cmp) macro_introsort1(name, less_no_arg, type, cmp)
#define macro_sort_less_arg(name, type, cmp) macro_introsort2(name, less_arg, type, cmp)
#define macro_sort_arg_less(name, type, cmp) macro_introsort2(name, arg_less, type, cmp)
#define macro_sort_less(name, type) macro_introsort1(name, less, type, unused)
#define macro_sort_cmp(name, type) macro_introsort1(name, cmp, type, unused)

#define macro_sort_compare_h_cmp_no_arg(name, type) macro_introsort_cmp_no_arg_h(name, type)
#define macro_sort_compare_h_cmp_arg(name, type) macro_introsort_cmp_arg_h(name, type)
#define macro_sort_compare_h_arg_cmp(name, type) macro_introsort_arg_cmp_h(name, type)
#define macro_sort_compare_h_less_no_arg(name, type) macro_introsort_less_no_arg_h(name, type)
#define macro_sort_compare_h_less_arg(name, type) macro_introsort_less_arg_h(name, type)
#define macro_sort_compare_h_arg_less(name, type) macro_introsort_arg_less_h(name, type)

#define macro_sort_compare_cmp_no_arg(name, type) macro_introsort_cmp_no_arg(name, type)
#define macro_sort_compare_cmp_arg(name, type) macro_introsort_cmp_arg(name, type)
#define macro_sort_compare_arg_cmp(name, type) macro_introsort_arg_cmp(name, type)
#define macro_sort_compare_less_no_arg(name, type) macro_introsort_less_no_arg(name, type)
#define macro_sort_compare_less_arg(name, type) macro_introsort_less_arg(name, type)
#define macro_sort_compare_arg_less(name, type) macro_introsort_arg_less(name, type)

#endif