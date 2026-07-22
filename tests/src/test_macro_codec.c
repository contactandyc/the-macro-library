// SPDX-FileCopyrightText: 2019–2026 Andy Curtis <contactandyc@gmail.com>
// SPDX-FileCopyrightText: 2024–2025 Knode.ai
// SPDX-License-Identifier: Apache-2.0
//
// Maintainer: Andy Curtis <contactandyc@gmail.com>

#include <stdint.h>
#include <string.h>
#include "the-macro-library/macro_codec.h"
#include "the-macro-library/macro_test.h"

MACRO_TEST(codec_fixed32_le_roundtrip) {
    uint8_t buf[4];
    uint32_t val = 0x12345678;

    macro_encode_u32_le(buf, val);
    MACRO_ASSERT_EQ_INT(buf[0], 0x78);
    MACRO_ASSERT_EQ_INT(buf[1], 0x56);
    MACRO_ASSERT_EQ_INT(buf[2], 0x34);
    MACRO_ASSERT_EQ_INT(buf[3], 0x12);

    uint32_t dec = macro_decode_u32_le(buf);
    MACRO_ASSERT_EQ_INT(dec, val);
}

MACRO_TEST(codec_fixed64_le_roundtrip) {
    uint8_t buf[8];
    uint64_t val = 0x1122334455667788ULL;

    macro_encode_u64_le(buf, val);
    MACRO_ASSERT_EQ_INT(buf[0], 0x88);
    MACRO_ASSERT_EQ_INT(buf[7], 0x11);

    uint64_t dec = macro_decode_u64_le(buf);
    /* Cannot use MACRO_ASSERT_EQ_INT for 64-bit values directly without truncation warning */
    MACRO_ASSERT_TRUE(dec == val);
}

MACRO_TEST(codec_varint32_encoding_lengths) {
    uint8_t buf[10];

    /* 1 byte */
    int len = macro_encode_varint32(buf, 127);
    MACRO_ASSERT_EQ_INT(len, 1);

    /* 2 bytes */
    len = macro_encode_varint32(buf, 128);
    MACRO_ASSERT_EQ_INT(len, 2);

    /* 3 bytes */
    len = macro_encode_varint32(buf, 16384);
    MACRO_ASSERT_EQ_INT(len, 3);
}

MACRO_TEST(codec_varint32_roundtrip_and_bounds) {
    uint8_t buf[10];
    uint32_t values[] = {0, 1, 127, 128, 16384, 0x7FFFFFFF, 0xFFFFFFFF};

    for (size_t i = 0; i < sizeof(values)/sizeof(values[0]); i++) {
        int enc_len = macro_encode_varint32(buf, values[i]);
        uint32_t dec_val;

        int dec_len = macro_decode_varint32(buf, 5, &dec_val);
        MACRO_ASSERT_EQ_INT(enc_len, dec_len);
        MACRO_ASSERT_TRUE(values[i] == dec_val);
    }

    /* Test boundary limits */
    macro_encode_varint32(buf, 16384); /* Takes 3 bytes */
    uint32_t tmp;
    MACRO_ASSERT_EQ_INT(macro_decode_varint32(buf, 2, &tmp), 0); /* Fails due to limit */
}

int main(void) {
    macro_test_case tests[64];
    size_t test_count = 0;

    MACRO_ADD(tests, codec_fixed32_le_roundtrip);
    MACRO_ADD(tests, codec_fixed64_le_roundtrip);
    MACRO_ADD(tests, codec_varint32_encoding_lengths);
    MACRO_ADD(tests, codec_varint32_roundtrip_and_bounds);

    macro_run_all("the-macro-library/macro_codec", tests, test_count);
    return 0;
}
