// SPDX-FileCopyrightText: 2019–2026 Andy Curtis <contactandyc@gmail.com>
// SPDX-FileCopyrightText: 2024–2025 Knode.ai
// SPDX-License-Identifier: Apache-2.0
//
// Maintainer: Andy Curtis <contactandyc@gmail.com>

#ifndef _MACRO_TO_H
#define _MACRO_TO_H

#include <stdbool.h>

/*
  This header declares the public API for conversion, bit, and time helpers.
  It also declares the new macro_try_to_* family of functions which return
  a success/failure flag and do not use defaults.
  The actual inline implementations are pulled in from
  "the-macro-library/src/macro_to.h" at the end.
*/

/* --------------------------------------------------------------------------
 * Time / date helpers
 * -------------------------------------------------------------------------- */

/* Returns the current time in microseconds since the Unix epoch. */
static inline long macro_to_time(void);

/* Converts a timestamp to "YYYY-MM-DD hh:mm:ss".
   Requires a buffer of at least 20 bytes. */
static char *macro_to_date_time(char *dest, time_t ts);

/* Converts a timestamp to "YYYY-MM-DD".
   Requires a buffer of at least 11 bytes. */
static char *macro_to_date(char *dest, time_t ts);

/* --------------------------------------------------------------------------
 * String passthrough
 * -------------------------------------------------------------------------- */

/* Returns value if not NULL, otherwise returns default_value. */
static inline const char *macro_to_string(const char *value, const char *default_value);

/* --------------------------------------------------------------------------
 * Bit helpers
 * -------------------------------------------------------------------------- */

/* Index of highest set bit in a 32-bit value, or -1 if zero. */
static inline int macro_highest_bit_index(uint32_t x);

/* Index of highest set bit in a 64-bit value, or -1 if zero. */
static inline int macro_highest_bit_index64(uint64_t x);

/* Index of lowest set bit in a 32-bit value, or -1 if zero. */
static inline int macro_lowest_bit_index(uint32_t x);

/* Index of lowest set bit in a 64-bit value, or -1 if zero. */
static inline int macro_lowest_bit_index64(uint64_t x);

/* Count of set bits in a 32-bit value. */
static inline int macro_bit_count(uint32_t x);

/* Count of set bits in a 64-bit value. */
static inline int macro_bit_count64(uint64_t x);

/* --------------------------------------------------------------------------
 * Default-returning conversions (overflow/invalid => default_value)
 * -------------------------------------------------------------------------- */

/* Parse base-10 int, returning default_value on failure or overflow. */
static inline int macro_to_int(const char *str, int default_value);

/* Parse base-10 long, returning default_value on failure or overflow. */
static inline long macro_to_long(const char *str, long default_value);

/* Parse base-10 int32, returning default_value on failure or overflow. */
static inline int32_t macro_to_int32(const char *str, int32_t default_value);

/* Parse base-10 uint32, returning default_value on failure or overflow. */
static inline uint32_t macro_to_uint32(const char *str, uint32_t default_value);

/* Parse base-10 int64, returning default_value on failure or overflow. */
static inline int64_t macro_to_int64(const char *str, int64_t default_value);

/* Parse base-10 uint64, returning default_value on failure or overflow. */
static inline uint64_t macro_to_uint64(const char *str, uint64_t default_value);

/* Parse float using strtof; must consume the entire string. */
static inline float macro_to_float(const char *str, float default_value);

/* Parse double using strtod; must consume the entire string. */
static inline double macro_to_double(const char *str, double default_value);

/* Boolean conversion with permissive rules based on default_value. */
static inline bool macro_to_bool(const char *str, bool default_value);

/* --------------------------------------------------------------------------
 * Try-conversions (return bool success, write result to *out)
 * -------------------------------------------------------------------------- */

/* Try parse base-10 int. */
static inline bool macro_try_to_int(const char *str, int *out);

/* Try parse base-10 long. */
static inline bool macro_try_to_long(const char *str, long *out);

/* Try parse base-10 int32_t. */
static inline bool macro_try_to_int32(const char *str, int32_t *out);

/* Try parse base-10 uint32_t. */
static inline bool macro_try_to_uint32(const char *str, uint32_t *out);

/* Try parse base-10 int64_t. */
static inline bool macro_try_to_int64(const char *str, int64_t *out);

/* Try parse base-10 uint64_t. */
static inline bool macro_try_to_uint64(const char *str, uint64_t *out);

/* Try parse float using strtof. */
static inline bool macro_try_to_float(const char *str, float *out);

/* Try parse double using strtod. */
static inline bool macro_try_to_double(const char *str, double *out);

/* Try parse strict boolean: only "true"/"false" (case-insensitive) or "1"/"0". */
static inline bool macro_try_to_bool(const char *str, bool *out);

/* --------------------------------------------------------------------------
 * Implementation include
 * -------------------------------------------------------------------------- */

#include "the-macro-library/src/macro_to.h"

#endif // _MACRO_TO_H
