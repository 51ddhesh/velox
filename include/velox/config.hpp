// Copyright 2026 51ddhesh
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once


#ifndef VELOX_FAST_FAIL
#define VELOX_FAST_FAIL 1
#endif

namespace velox {
    inline constexpr bool kFailFast = (VELOX_FAST_FAIL != 0);
} // namespace velox
