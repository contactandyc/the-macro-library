# README.macro\_time.md — High-Resolution Timing Utilities

`macro_time.h` provides **tiny, inline, cross-platform (POSIX)** helpers for measuring and manipulating monotonic time with nanosecond resolution.
It’s designed for benchmarking, profiling, and scheduling within the macro library or your own code.

---

## Why use `macro_time.h`?

* **High resolution**: Uses `clock_gettime(CLOCK_MONOTONIC)` for nanosecond precision.
* **Monotonic**: Immune to system clock changes (e.g., NTP sync, daylight savings).
* **Lightweight**: Three small static inline functions, no heap allocations.
* **Self-contained**: No external dependencies beyond `<time.h>` and `<stdint.h>`.

---

## API Overview

### `uint64_t macro_now(void)`

Returns the current monotonic time in **nanoseconds** since an arbitrary epoch (not related to wall clock time).

**Example:**

```c
uint64_t t0 = macro_now();
/* do something */
uint64_t t1 = macro_now();
printf("Elapsed: %.6f seconds\n", macro_time_diff(t1, t0));
```

---

### `uint64_t macro_now_add_seconds(int32_t seconds)`

Returns the current monotonic time **plus** a specified number of seconds, in nanoseconds.

Useful for scheduling:

```c
uint64_t deadline = macro_now_add_seconds(5); // 5 seconds from now
while (macro_now() < deadline) {
    /* busy wait or poll something */
}
```

---

### `double macro_time_diff(uint64_t end, uint64_t start)`

Returns the **elapsed time in seconds** between two `macro_now()` readings.

* `end` and `start` are both in nanoseconds.
* Output is a `double` representing fractional seconds.

**Example:**

```c
uint64_t start_ns = macro_now();
/* some operation */
uint64_t end_ns   = macro_now();
double elapsed_s  = macro_time_diff(end_ns, start_ns);
printf("Took %.9f seconds\n", elapsed_s);
```

---

## Units & Conversions

* All raw time values from `macro_now()` and `macro_now_add_seconds()` are in **nanoseconds** (`uint64_t`).
* `macro_time_diff()` converts nanoseconds to seconds for human-friendly printing.

---

## Practical Use Cases

1. **Benchmarking Functions**

   ```c
   uint64_t t0 = macro_now();
   my_function();
   uint64_t t1 = macro_now();
   printf("my_function took %.3f ms\n", macro_time_diff(t1, t0) * 1000);
   ```

2. **Implementing Timeouts**

   ```c
   uint64_t timeout = macro_now_add_seconds(10);
   while (macro_now() < timeout) {
       check_status();
   }
   ```

3. **Animation / Frame Timing**

   ```c
   uint64_t frame_start = macro_now();
   render_frame();
   double frame_time = macro_time_diff(macro_now(), frame_start);
   sleep_until_next_frame(frame_time);
   ```

---

## Notes & Limitations

* **Platform**: Relies on `clock_gettime(CLOCK_MONOTONIC)` — available on Linux, macOS (10.12+), and many UNIX-like OSes.
  On older macOS versions, you’ll need to shim with `mach_absolute_time()` if porting.
* **Overflow**: `uint64_t` nanoseconds will wrap after \~584 years — practically irrelevant.
* **Not wall time**: Values are relative to an unspecified start point. Use `CLOCK_REALTIME` (not provided here) if you need actual calendar timestamps.

---

## Full API Summary

```c
#include "the-macro-library/macro_time.h"

/* Get current monotonic time in nanoseconds */
uint64_t macro_now(void);

/* Get monotonic time in nanoseconds, offset by N seconds */
uint64_t macro_now_add_seconds(int32_t seconds);

/* Get difference between two monotonic timestamps in seconds */
double macro_time_diff(uint64_t end, uint64_t start);
```
