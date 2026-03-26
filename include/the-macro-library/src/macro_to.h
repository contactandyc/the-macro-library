// SPDX-FileCopyrightText: 2019–2026 Andy Curtis <contactandyc@gmail.com>
// SPDX-FileCopyrightText: 2024–2025 Knode.ai
// SPDX-License-Identifier: Apache-2.0
//
// Maintainer: Andy Curtis <contactandyc@gmail.com>

#ifndef _macro_to_H
#define _macro_to_H

#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/time.h>
#include <limits.h>
#include <strings.h>

/* ---------------- Bit helpers ---------------- */

static inline int macro_highest_bit_index(uint32_t x) {
    if (x == 0) return -1;
#ifdef __GNUC__
    return 31 - __builtin_clz(x);
#else
    int index = 0;
    while (x >>= 1) index++;
    return index;
#endif
}

static inline int macro_highest_bit_index64(uint64_t x) {
    if (x == 0) return -1;
#ifdef __GNUC__
    return 63 - __builtin_clzll(x);
#else
    int index = 0;
    while (x >>= 1) index++;
    return index;
#endif
}

static inline int macro_lowest_bit_index(uint32_t x) {
    if (x == 0) return -1;
#ifdef __GNUC__
    return __builtin_ctz(x);
#else
    int index = 0;
    while ((x & 1u) == 0u) { x >>= 1; index++; }
    return index;
#endif
}

static inline int macro_bit_count(uint32_t x) {
#ifdef __GNUC__
    return __builtin_popcount(x);
#else
    int count = 0;
    while (x) { count += (x & 1u); x >>= 1; }
    return count;
#endif
}

static inline int macro_lowest_bit_index64(uint64_t x) {
    if (x == 0) return -1;
#ifdef __GNUC__
    return __builtin_ctzll(x);
#else
    int index = 0;
    while ((x & 1ull) == 0ull) { x >>= 1; index++; }
    return index;
#endif
}

static inline int macro_bit_count64(uint64_t x) {
#ifdef __GNUC__
    return __builtin_popcountll(x);
#else
    int count = 0;
    while (x) { count += (int)(x & 1ull); x >>= 1; }
    return count;
#endif
}

/* ---------------- Time / date helpers ---------------- */

static inline long macro_to_time(void) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (tv.tv_sec * 1000000L) + tv.tv_usec;
}

static inline char *macro_to_date_time(char *dest, time_t ts) {
  struct tm t;
  gmtime_r(&ts, &t);
  sprintf(dest, "%04d-%02d-%02d %02d:%02d:%02d",
          t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
          t.tm_hour, t.tm_min, t.tm_sec);
  return dest;
}

static inline char *macro_to_date(char *dest, time_t ts) {
  struct tm t;
  gmtime_r(&ts, &t);
  sprintf(dest, "%04d-%02d-%02d",
          t.tm_year + 1900, t.tm_mon + 1, t.tm_mday);
  return dest;
}

/* ---------------- String passthrough ---------------- */

static inline const char *macro_to_string(const char *value, const char *default_value) {
    return value ? value : default_value;
}

/* ======================================================================
   Internal helpers for overflow-safe base-10 parsing
   ====================================================================== */

static inline int macro__is_digit(unsigned char ch) {
    return (ch >= '0' && ch <= '9');
}

/* Parse unsigned decimal into uint64_t with an inclusive limit.
   Returns 1 on success, 0 on invalid char/empty/overflow; *out is set on success. */
static inline int macro__parse_u64_with_limit(const char *str, uint64_t limit, uint64_t *out) {
    if (!str || !*str) return 0;

    uint64_t acc = 0;
    for (const unsigned char *p = (const unsigned char *)str; *p; ++p) {
        if (!macro__is_digit(*p)) return 0;
        uint64_t digit = (uint64_t)(*p - '0');
        if (acc > limit / 10u || (acc == limit / 10u && digit > (limit % 10u))) {
            return 0; /* overflow */
        }
        acc = acc * 10u + digit;
    }
    *out = acc;
    return 1;
}

/* Parse unsigned decimal into uint32_t with an inclusive limit.
   Returns 1 on success, 0 otherwise. */
static inline int macro__parse_u32_with_limit(const char *str, uint32_t limit, uint32_t *out) {
    if (!str || !*str) return 0;

    uint32_t acc = 0;
    for (const unsigned char *p = (const unsigned char *)str; *p; ++p) {
        if (!macro__is_digit(*p)) return 0;
        uint32_t digit = (uint32_t)(*p - '0');
        if (acc > limit / 10u || (acc == limit / 10u && digit > (limit % 10u))) {
            return 0; /* overflow */
        }
        acc = acc * 10u + digit;
    }
    *out = acc;
    return 1;
}

/* Parse signed decimal for a type with MAX, MIN using uint64 magnitude checks.
   Returns 1 on success, 0 otherwise; *out is set on success. */
static inline int macro__parse_i64_bounded(const char *str, int64_t tmin, int64_t tmax, int64_t *out) {
    if (!str || !*str) return 0;

    int neg = (str[0] == '-');
    const char *p = str + (neg ? 1 : 0);
    if (!*p) return 0; /* just '-' is invalid */

    /* Compute magnitude limits:
       For positive: limit = tmax
       For negative: limit = (uint64_t)tmax + 1  (i.e., |tmin|) */
    uint64_t limit_mag = neg ? ((uint64_t)tmax + 1ull) : (uint64_t)tmax;

    uint64_t mag = 0;
    for (const unsigned char *q = (const unsigned char *)p; *q; ++q) {
        if (!macro__is_digit(*q)) return 0;
        uint64_t digit = (uint64_t)(*q - '0');
        if (mag > limit_mag / 10ull || (mag == limit_mag / 10ull && digit > (limit_mag % 10ull))) {
            return 0; /* overflow magnitude */
        }
        mag = mag * 10ull + digit;
    }

    if (neg) {
        if (mag == ((uint64_t)tmax + 1ull)) {
            *out = tmin; /* exact MIN */
            return 1;
        }
        /* otherwise -mag fits within [-tmax, -1] */
        *out = -(int64_t)mag;
        return 1;
    } else {
        *out = (int64_t)mag; /* mag <= tmax ensured by limit */
        return 1;
    }
}

/* Same idea for 32-bit signed. */
static inline int macro__parse_i32_bounded(const char *str, int32_t tmin, int32_t tmax, int32_t *out) {
    if (!str || !*str) return 0;

    int neg = (str[0] == '-');
    const char *p = str + (neg ? 1 : 0);
    if (!*p) return 0;

    uint32_t limit_mag = neg ? ((uint32_t)tmax + 1u) : (uint32_t)tmax;

    uint32_t mag = 0;
    for (const unsigned char *q = (const unsigned char *)p; *q; ++q) {
        if (!macro__is_digit(*q)) return 0;
        uint32_t digit = (uint32_t)(*q - '0');
        if (mag > limit_mag / 10u || (mag == limit_mag / 10u && digit > (limit_mag % 10u))) {
            return 0; /* overflow magnitude */
        }
        mag = mag * 10u + digit;
    }

    if (neg) {
        if (mag == ((uint32_t)tmax + 1u)) { *out = tmin; return 1; }
        *out = -(int32_t)mag;
        return 1;
    } else {
        *out = (int32_t)mag;
        return 1;
    }
}

/* ---------------- Strict integer conversions (overflow/underflow => default) ---------------- */

static inline int macro_to_int(const char *str, int default_value) {
#if INT_MAX == 2147483647
    int32_t out;
    if (macro__parse_i32_bounded(str, INT_MIN, INT_MAX, &out)) return (int)out;
    return default_value;
#else
    int64_t out;
    if (macro__parse_i64_bounded(str, INT_MIN, INT_MAX, &out)) return (int)out;
    return default_value;
#endif
}

static inline long macro_to_long(const char *str, long default_value) {
#if LONG_MAX == 2147483647L
    int32_t out;
    if (macro__parse_i32_bounded(str, LONG_MIN, LONG_MAX, &out)) return (long)out;
    return default_value;
#else
    int64_t out;
    if (macro__parse_i64_bounded(str, LONG_MIN, LONG_MAX, &out)) return (long)out;
    return default_value;
#endif
}

static inline int32_t macro_to_int32(const char *str, int32_t default_value) {
    int32_t out;
    if (macro__parse_i32_bounded(str, INT32_MIN, INT32_MAX, &out)) return out;
    return default_value;
}

static inline uint32_t macro_to_uint32(const char *str, uint32_t default_value) {
    uint32_t out;
    if (macro__parse_u32_with_limit(str, UINT32_MAX, &out)) return out;
    return default_value;
}

static inline int64_t macro_to_int64(const char *str, int64_t default_value) {
    int64_t out;
    if (macro__parse_i64_bounded(str, INT64_MIN, INT64_MAX, &out)) return out;
    return default_value;
}

static inline uint64_t macro_to_uint64(const char *str, uint64_t default_value) {
    uint64_t out;
    if (macro__parse_u64_with_limit(str, UINT64_MAX, &out)) return out;
    return default_value;
}

/* ---------------- Floating-point (unchanged strictness using strto*) ---------------- */

static inline float macro_to_float(const char *str, float default_value) {
    if (!str) return default_value;
    errno = 0;
    char *end = NULL;
    float v = strtof(str, &end);
    if (errno != 0 || end == str || *end != '\0') return default_value;
    return v;
}

static inline double macro_to_double(const char *str, double default_value) {
    if (!str) return default_value;
    errno = 0;
    char *end = NULL;
    double v = strtod(str, &end);
    if (errno != 0 || end == str || *end != '\0') return default_value;
    return v;
}

/* ---------------- Bool (kept as in your current semantics) ---------------- */

static inline bool macro_to_bool(const char *str, bool default_value) {
    if (!str) return default_value;

    if (default_value == true) {
        if (str[0] == 'f' || str[0] == '0' || str[0] == 'n' ||
            str[0] == 'F' || str[0] == 'N')
            return false;
        return true;
    } else {
        if (str[0] == 't' || str[0] == '1' || str[0] == 'y' ||
            str[0] == 'T' || str[0] == 'Y')
            return true;
        return false;
    }
}


/* ---------------- Overflow-safe integer parsing with validity flag ---------------- */

static inline bool macro_try_to_int(const char *str, int *out) {
#if INT_MAX == 2147483647
    int32_t tmp;
    if (macro__parse_i32_bounded(str, INT_MIN, INT_MAX, &tmp)) { *out = (int)tmp; return true; }
    return false;
#else
    int64_t tmp;
    if (macro__parse_i64_bounded(str, INT_MIN, INT_MAX, &tmp)) { *out = (int)tmp; return true; }
    return false;
#endif
}

static inline bool macro_try_to_long(const char *str, long *out) {
#if LONG_MAX == 2147483647L
    int32_t tmp;
    if (macro__parse_i32_bounded(str, LONG_MIN, LONG_MAX, &tmp)) { *out = (long)tmp; return true; }
    return false;
#else
    int64_t tmp;
    if (macro__parse_i64_bounded(str, LONG_MIN, LONG_MAX, &tmp)) { *out = (long)tmp; return true; }
    return false;
#endif
}

static inline bool macro_try_to_int32(const char *str, int32_t *out) {
    return macro__parse_i32_bounded(str, INT32_MIN, INT32_MAX, out);
}

static inline bool macro_try_to_uint32(const char *str, uint32_t *out) {
    return macro__parse_u32_with_limit(str, UINT32_MAX, out);
}

static inline bool macro_try_to_int64(const char *str, int64_t *out) {
    return macro__parse_i64_bounded(str, INT64_MIN, INT64_MAX, out);
}

static inline bool macro_try_to_uint64(const char *str, uint64_t *out) {
    return macro__parse_u64_with_limit(str, UINT64_MAX, out);
}

/* ---------------- Floats/Doubles with validity flag ---------------- */

static inline bool macro_try_to_float(const char *str, float *out) {
    if (!str) return false;
    errno = 0;
    char *end = NULL;
    float v = strtof(str, &end);
    if (errno != 0 || end == str || *end != '\0') return false;
    *out = v;
    return true;
}

static inline bool macro_try_to_double(const char *str, double *out) {
    if (!str) return false;
    errno = 0;
    char *end = NULL;
    double v = strtod(str, &end);
    if (errno != 0 || end == str || *end != '\0') return false;
    *out = v;
    return true;
}

/* ---------------- Bool with validity flag ---------------- */

static inline bool macro_try_to_bool(const char *str, bool *out) {
    if (!str) return false;
    /* stricter: only exact matches "true" / "false" (case-insensitive) or "0"/"1" */
    if (str[0] == '1' && str[1] == '\0') { *out = true; return true; }
    if (str[0] == '0' && str[1] == '\0') { *out = false; return true; }
    if (strcasecmp(str, "true") == 0)  { *out = true; return true; }
    if (strcasecmp(str, "false") == 0) { *out = false; return true; }
    if (strcasecmp(str, "yes") == 0)  { *out = true; return true; }
    if (strcasecmp(str, "no") == 0)  { *out = false; return true; }
    return false;
}

#endif /* _macro_to_H */
