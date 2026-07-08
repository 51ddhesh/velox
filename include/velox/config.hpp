// Copyright 2026 51ddhesh
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once


#include <cstdint>

#ifndef VELOX_FAIL_FAST
    #define VELOX_FAIL_FAST 1
#endif

namespace velox {

inline constexpr bool kFailFast = (VELOX_FAIL_FAST != 0);

} // namespace velox

#if defined(__clang__) || defined(__GNUC__)
    #define VELOX_ALWAYS_INLINE __attribute__((always_inline)) inline
    #define VELOX_COLD __attribute__((cold))
#else
    #define VELOX_ALWAYS_INLINE inline
    #define VELOX_COLD inline
#endif

#if defined(__clang__)
    #define VELOX_ASSUME(expr) __builtin_assume(expr)
#elif defined(__GNUC__)
    #define VELOX_ASSUME(expr) do { if (!(expr)) __builtin_unreachable(); } while (0)
#else
    #define VELOX_ASSUME(expr) ((void)0)
#endif
