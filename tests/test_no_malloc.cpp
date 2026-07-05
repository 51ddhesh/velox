// Copyright 2026 51ddhesh
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <atomic>
#include <cstddef>

#if defined(VELOX_HAVE_MALLOC_WRAP) && VELOX_HAVE_MALLOC_WRAP

static std::atomic<std::uint64_t> g_malloc_calls{0};

extern "C" void* __real_malloc(std::size_t);
extern "C" void  __real_free(void*);

extern "C" void* __wrap_malloc(std::size_t n) {
  g_malloc_calls.fetch_add(1, std::memory_order_relaxed);
  return __real_malloc(n);
}

extern "C" void __wrap_free(void* p) {
  __real_free(p);
}

TEST_CASE("No malloc in a tight region (best-effort check)") {
  const auto before = g_malloc_calls.load(std::memory_order_relaxed);

  // Do nothing that should allocate.
  // (As we add components, we'll exercise their hot-path methods here.)

  const auto after = g_malloc_calls.load(std::memory_order_relaxed);
  CHECK(after == before);
}

#else

TEST_CASE("No malloc test skipped (no linker wrap support)") {
  CHECK(true);
}

#endif