# README.macro\_to.md — Tiny “to/from” & bit helpers

`macro_to.h` bundles small, dependency‑free helpers in three areas:

1. **Time utilities** (wall‑clock microseconds, UTC formatting)
2. **String → number** conversions with safe defaults (overflow ⇒ default)
3. **Bit operations** (highest/lowest set bit, popcount)

Public header: `the-macro-library/macro_to.h`
Inline impl:   `the-macro-library/src/macro_to.h`

---

## Quick start

```c
#include "the-macro-library/macro_to.h"

void demo(void) {
    // 1) Wall-clock microseconds since Unix epoch (not monotonic)
    long t_us = macro_to_time();

    // 2) UTC formatting (caller allocates the buffers below)
    char dt[20];  // "YYYY-MM-DD hh:mm:ss" + '\0'
    char d[11];   // "YYYY-MM-DD" + '\0'
    time_t now = time(NULL);
    macro_to_date_time(dt, now);
    macro_to_date(d, now);

    // 3) Strict string → number with default fallback
    int      port  = macro_to_int(getenv("PORT"), 8080);     // invalid/overflow -> 8080
    uint64_t limit = macro_to_uint64("18446744073709551615", 0); // UINT64_MAX
    double   r     = macro_to_double("0.75e+1", 1.0);        // 7.5
    bool     dbg   = macro_to_bool(getenv("DEBUG"), false);  // permissive first-char rules

    // 4) Exact parsing with success flag (no defaults)
    int v;
    if (macro_try_to_int("123", &v)) { /* use v */ }

    // 5) Bit ops
    int hi = macro_highest_bit_index(0x8100u);   // 15
    int lo = macro_lowest_bit_index(0x8100u);    // 8
    int pc = macro_bit_count(0xF0F0u);           // 8
}
```

---

## API

### Time

* `long macro_to_time(void);`
  Wall‑clock microseconds since Unix epoch (`gettimeofday`). Not monotonic.

* `char *macro_to_date_time(char *dst, time_t ts);`
  UTC `"YYYY-MM-DD hh:mm:ss"`. `dst` ≥ 20 bytes. Returns `dst`.

* `char *macro_to_date(char *dst, time_t ts);`
  UTC `"YYYY-MM-DD"`. `dst` ≥ 11 bytes. Returns `dst`.

### String → number (strict with default fallback)

If input is `NULL`, malformed, or **out of range**, the **default value is returned**.

* `const char *macro_to_string(const char *s, const char *def);`

* `int      macro_to_int     (const char *s, int      def);`

* `long     macro_to_long    (const char *s, long     def);`

* `int32_t  macro_to_int32   (const char *s, int32_t  def);`

* `uint32_t macro_to_uint32  (const char *s, uint32_t def);`

* `int64_t  macro_to_int64   (const char *s, int64_t  def);`

* `uint64_t macro_to_uint64  (const char *s, uint64_t def);`

* `float    macro_to_float   (const char *s, float    def);`

* `double   macro_to_double  (const char *s, double   def);`

* `bool     macro_to_bool    (const char *s, bool     def);`

**Integer parsing rules (for the `macro_to_*` family):**

* Decimal only. No spaces, no `+`, no hex, no underscores.
* Signed variants accept an optional leading `-`; unsigned do not accept signs.
* Entire string must match the form; any extra characters cause fallback.
* **Overflow/underflow ⇒ return the default.**

**Floating‑point (`strtof`/`strtod`):**

* Decimal and exponent notation (e.g., `"3.14"`, `"1e-6"`).
* Requires full consumption of the string; trailing junk ⇒ default.
* Honors the C locale’s decimal separator.

**Booleans (permissive):**

* `macro_to_bool(s, def)` checks only the first character:

  * If `def == true`: `f/F/0/n/N` ⇒ `false`, else `true`.
  * If `def == false`: `t/T/1/y/Y` ⇒ `true`, else `false`.

### Try‑conversions (strict, no defaults)

These return `true` on success and write the parsed value to `*out`. On failure (NULL, junk, overflow, underflow), return `false` and leave `*out` unspecified.

* `bool macro_try_to_int     (const char *s, int      *out);`
* `bool macro_try_to_long    (const char *s, long     *out);`
* `bool macro_try_to_int32   (const char *s, int32_t  *out);`
* `bool macro_try_to_uint32  (const char *s, uint32_t *out);`
* `bool macro_try_to_int64   (const char *s, int64_t  *out);`
* `bool macro_try_to_uint64  (const char *s, uint64_t *out);`
* `bool macro_try_to_float   (const char *s, float    *out);`
* `bool macro_try_to_double  (const char *s, double   *out);`
* `bool macro_try_to_bool    (const char *s, bool     *out);`

**Notes:**

* Integer rules match the `macro_to_*` family and perform explicit range checks.
* Float/double use `strtof/strtod` with full‑string consumption.
* `macro_try_to_bool` is **strict**: accepts `"true / yes"`/`"false / no"` (case‑insensitive) and `"1"`/`"0"` only.

### Bit operations

* `int macro_highest_bit_index(uint32_t x);`

* `int macro_highest_bit_index64(uint64_t x);`
  Index (0 = LSB) of the most significant set bit; **-1 if x == 0**.

* `int macro_lowest_bit_index(uint32_t x);`

* `int macro_lowest_bit_index64(uint64_t x);`
  Index (0 = LSB) of the least significant set bit; **-1 if x == 0**.

* `int macro_bit_count(uint32_t x);`

* `int macro_bit_count64(uint64_t x);`
  Population count (set bits).

Builtins (`__builtin_clz/ctz/popcount`) are used when available; otherwise simple loops.

---

## Behavior notes

* **Overflow policy (new):** all integer conversions now return the **default** on overflow/underflow.
  Previously, extremely large values could wrap during accumulation.
* `macro_to_bool` remains permissive (first‑char), while `macro_try_to_bool` is strict.
* `macro_to_time()` is wall‑clock, not monotonic. Use a monotonic clock for timing intervals.

---

## Examples

### Environment with defaults

```c
const char *host = macro_to_string(getenv("HOST"), "127.0.0.1");
int         port = macro_to_int(getenv("PORT"), 8080);
bool        tls  = macro_to_bool(getenv("TLS"), false);
```

### Strict parse with error handling

```c
uint64_t cap;
if (!macro_try_to_uint64(env_cap_str, &cap)) {
    cap = 1000; // fallback path on parse failure or overflow
}
```

### Signed/unsigned boundaries

```c
int32_t a = macro_to_int32("2147483647", -1);       // OK
int32_t b = macro_to_int32("2147483648", -1);       // overflow -> -1

uint32_t u = macro_to_uint32("4294967295", 0);      // OK
uint32_t v = macro_to_uint32("4294967296", 0);      // overflow -> 0
```

### Booleans: permissive vs strict

```c
bool b1 = macro_to_bool("yes", false);  // true (permissive)
bool b2;
bool ok = macro_try_to_bool("yes", &b2); // false (strict), b2 unspecified
```

### Bit ops

```c
uint64_t x = 0x8000000000000010ull; // bits 4 and 63 set
int hi = macro_highest_bit_index64(x); // 63
int lo = macro_lowest_bit_index64(x);  // 4
int pc = macro_bit_count64(x);         // 2
```

---

## Portability

* Uses `gettimeofday` and `gmtime_r` (POSIX). On Windows, provide shims (`QueryPerformanceCounter`, `gmtime_s`) if needed.
* All functions are `static inline`; no linking step is required.

---

## Upgrading from older versions

* Integer conversions now fail closed: **overflow/underflow returns the default** instead of producing wrapped results.
* Add `macro_try_to_*` if you need a success/failure signal without defaults.
* Existing permissive boolean behavior is unchanged; use `macro_try_to_bool` for strict `true/false` or `1/0` parsing.
