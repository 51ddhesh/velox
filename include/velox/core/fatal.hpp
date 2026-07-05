// Copyright 2026 51ddhesh
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once


#include <cerrno>
#include <cstdio>
#include <cstdlib>

namespace velox::core {

[[noreturn]] inline void fatal(const char* msg) noexcept {
    std::fprintf(stderr, "[velox fatal]: %s\n", msg);
    std::fflush(stderr);
    std::abort();
}

[[noreturn]] inline void fatal_errno(const char* ctx) noexcept {
    std::fprintf(stderr, "[velox fatal]: %s (errno=%d)\n", ctx, errno);
    std::fflush(stderr);
    std::abort();
}

} // velox::core
