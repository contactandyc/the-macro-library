`macro_codec` provides lightweight, inline C functions for safely and portably encoding/decoding integers. It handles **fixed-width little-endian** serialization and base-128 **variable-length integers (varints)**.

Because these are implemented as `static inline` functions, they provide type safety, avoid macro-expansion side effects, and are easily optimized by the compiler.

---

## Quick start

### 1) Fixed-Width Little-Endian

Use these when you need deterministic byte sizes (e.g., writing exact 4-byte or 8-byte binary formats). They are safe against unaligned memory access and host-endianness differences.

```c
#include "the-macro-library/macro_codec.h"

uint8_t buffer[12];

/* Encode */
macro_encode_u32_le(buffer, 42);             // Writes 4 bytes
macro_encode_u64_le(buffer + 4, 123456789);  // Writes 8 bytes

/* Decode */
uint32_t val32 = macro_decode_u32_le(buffer);
uint64_t val64 = macro_decode_u64_le(buffer + 4);

```

### 2) Varint32 (Variable-Length Integers)

Varints compress smaller integers into fewer bytes (1 to 5 bytes for a 32-bit integer) using a base-128 format where the MSB of each byte acts as a continuation bit.

```c
uint8_t var_buf[10];
uint32_t my_value = 300;

/* Encode */
int bytes_written = macro_encode_varint32(var_buf, my_value);
// 300 takes 2 bytes to encode.

/* Decode */
uint32_t decoded_val;
int bytes_read = macro_decode_varint32(var_buf, sizeof(var_buf), &decoded_val);

if (bytes_read == 0) {
    // Handle error: malformed data or insufficient bounds
}

```

---

## API surface

### Fixed-Width Operations

* `void macro_encode_u32_le(uint8_t *dst, uint32_t v)`
* `uint32_t macro_decode_u32_le(const uint8_t *src)`
* `void macro_encode_u64_le(uint8_t *dst, uint64_t v)`
* `uint64_t macro_decode_u64_le(const uint8_t *src)`

### Varint Operations

* `int macro_encode_varint32(uint8_t *dst, uint32_t value)`
* **Returns:** The number of bytes written (1 to 5).


* `int macro_decode_varint32(const uint8_t *src, uint32_t limit, uint32_t *value)`
* **Returns:** The number of bytes read.
* **Safety:** Returns `0` if the varint is malformed (no terminal byte found within 5 bytes) or if it exceeds the provided `limit` bound.
