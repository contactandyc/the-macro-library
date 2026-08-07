// SPDX-FileCopyrightText: 2019–2026 Andy Curtis <contactandyc@gmail.com>
// SPDX-FileCopyrightText: 2024–2025 Knode.ai
// SPDX-License-Identifier: Apache-2.0
//
// Maintainer: Andy Curtis <contactandyc@gmail.com>

#ifndef MACRO_SIMD_H
#define MACRO_SIMD_H

#if defined(__AVX512F__)
    #define MACRO_SIMD_VERSION "AVX-512"
#elif defined(__AVX2__)
    #define MACRO_SIMD_VERSION "AVX2"
#elif defined(__AVX__)
    #define MACRO_SIMD_VERSION "AVX"
#elif defined(__ARM_NEON) || defined(__aarch64__)
    #define MACRO_SIMD_VERSION "ARM NEON"
#else
    #define MACRO_SIMD_VERSION "Scalar (No SIMD)"
#endif

#endif // MACRO_SIMD_H
