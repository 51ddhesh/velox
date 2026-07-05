// Copyright 2026 51ddhesh
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <velox/memory/huge_page.hpp>

TEST_CASE("HugePageMapping basic mapping (no lock, no huge)") {
  velox::memory::HugePageMapping m(64 * 1024,
                                  velox::memory::HugePageMode::Disable,
                                  velox::memory::LockMode::DontLock);
  CHECK(m.data() != nullptr);
  CHECK(m.size() >= 64 * 1024);
}