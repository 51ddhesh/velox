// Copyright 2026 51ddhesh
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <velox/memory/arena.hpp>
#include <cstdint>

static bool is_aligned(const void* p, std::size_t a) {
  return (reinterpret_cast<std::uintptr_t>(p) & (a - 1)) == 0;
}

TEST_CASE("Arena allocates aligned blocks") {
  velox::memory::Arena a(4096,
                        velox::memory::HugePageMode::Disable,
                        velox::memory::LockMode::DontLock);

  void* p1 = a.allocate_bytes(32, 64);
  CHECK(p1 != nullptr);
  CHECK(is_aligned(p1, 64));

  void* p2 = a.allocate_bytes(128, 16);
  CHECK(p2 != nullptr);
  CHECK(is_aligned(p2, 16));

  void* p3 = a.allocate_bytes(4096, 8);
  CHECK(p3 == nullptr); // should exhaust
}