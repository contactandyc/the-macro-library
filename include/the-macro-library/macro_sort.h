// SPDX-FileCopyrightText: 2019–2026 Andy Curtis <contactandyc@gmail.com>
// SPDX-FileCopyrightText: 2024–2025 Knode.ai
// SPDX-License-Identifier: Apache-2.0
//
// Maintainer: Andy Curtis <contactandyc@gmail.com>

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
