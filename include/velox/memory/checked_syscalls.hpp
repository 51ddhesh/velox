// Copyright 2026 51ddhesh
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once


#include <cerrno>

#include <velox/config.hpp>
#include <velox/core/fatal.hpp>

namespace velox::memory {

inline void check_syscall(int rc, const char* what) noexcept {
    if (rc == 0) [[likely]] return;

    if (velox::kFailFast) velox::core::fatal_errno(what);
    velox::core::fatal_errno(what);
}

inline void* check_mmap(void* p, const char* what) noexcept {
    if (p != reinterpret_cast<void*>(-1)) [[likely]] return p;
    if (velox::kFailFast) velox::core::fatal_errno(what);
    velox::core::fatal_errno(what);
}

} // namespace velox::memory