#ifndef _MACRO_TO_H
#define _MACRO_TO_H

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

// Converts a time stamp to YYYY-MM-DD hh:mm:ss (requires a 20 byte dest)
static char *macro_to_date_time(char *dest, time_t ts);

// Converts a time stamp to YYYY-MM-DD (requires a 11 byte dest)
static char *macro_to_date(char *dest, time_t ts);

// Converts a string to an int. Handles negative numbers.
static inline int macro_to_int(const char *str, int default_value);

// Converts a string to a 32-bit integer (int32_t). Supports negative numbers.
static inline int32_t macro_to_int32(const char *str, int32_t default_value);

// Converts a string to an unsigned 32-bit integer (uint32_t).
static inline uint32_t macro_to_uint32(const char *str, uint32_t default_value);

// Converts a string to a 64-bit integer (int64_t). Also handles negative numbers.
static inline int64_t macro_to_int64(const char *str, int64_t default_value);

// Converts a string to an unsigned 64-bit integer (uint64_t).
static inline uint64_t macro_to_uint64(const char *str, uint64_t default_value);

// Converts a string to a float. Handles decimal and exponential notation.
static inline float macro_to_float(const char *str, float default_value);

// Converts a string to a double. Supports decimal and exponential notation.
static inline double macro_to_double(const char *str, double default_value);

// Converts a string to a bool. Interprets several representations of true/false.
static inline bool macro_to_bool(const char *str, bool default_value);

#include "the-macro-library/src/macro_to.h"

#endif // _MACRO_TO_H
