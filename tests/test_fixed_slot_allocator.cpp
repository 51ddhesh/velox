// Copyright 2026 51ddhesh
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <velox/memory/fixed_slot_allocator.hpp>

TEST_CASE("FixedSizeSlotAllocator alloc/dealloc") {
  velox::memory::FixedSizeSlotAllocator<int, 128> pool(
      velox::memory::HugePageMode::Disable,
      velox::memory::LockMode::DontLock);

  int* ptrs[128]{};

  for (int i = 0; i < 128; ++i) {
    ptrs[i] = pool.allocate();
    CHECK(ptrs[i] != nullptr);
    *ptrs[i] = i;
  }

  CHECK(pool.available() == 0);

  // Exhaustion is fail-fast by default; don't call allocate() again here.

  pool.deallocate(ptrs[127]);
  CHECK(pool.available() == 1);

  int* p = pool.allocate();
  CHECK(p == ptrs[127]); // LIFO behavior
}