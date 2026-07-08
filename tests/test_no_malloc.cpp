// Copyright 2026 51ddhesh
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <atomic>
#include <array>
#include <cstddef>
#include <cstdint>

#include <velox/memory/fixed_slot_allocator.hpp>
#include <velox/memory/arena.hpp>

#if defined(VELOX_HAVE_MALLOC_WRAP) && VELOX_HAVE_MALLOC_WRAP

static std::atomic<std::uint64_t> g_malloc_calls{0};

extern "C" void* __real_malloc(std::size_t);
extern "C" void  __real_free(void*);

extern "C" void* __wrap_malloc(std::size_t n) {
    g_malloc_calls.fetch_add(1, std::memory_order_relaxed);
    return __real_malloc(n);
}
extern "C" void __wrap_free(void* p) { __real_free(p); }

TEST_CASE("No malloc during allocator hot-path calls") {
    velox::memory::FixedSizeSlotAllocator<int, 4096> pool(
        velox::memory::HugePageMode::Disable,
        velox::memory::LockMode::DontLock
    );

    velox::memory::Arena arena(1 << 20,
                             velox::memory::HugePageMode::Disable,
                             velox::memory::LockMode::DontLock);

    std::array<int*, 4096> ptrs{};

    const auto before = g_malloc_calls.load(std::memory_order_relaxed);

    for (int i = 0; i < 4096; ++i) {
        ptrs[static_cast<size_t>(i)] = pool.allocate();
        CHECK(ptrs[static_cast<size_t>(i)] != nullptr);
        *ptrs[static_cast<size_t>(i)] = i;
    }

    for (int i = 0; i < 4096; ++i) {
        pool.deallocate(ptrs[static_cast<size_t>(i)]);
    }

    for (int i = 0; i < 1000; ++i) {
        auto* x = arena.allocate<std::uint64_t>(8);
        CHECK(x != nullptr);
        x[0] = 123;
    }

    const auto after = g_malloc_calls.load(std::memory_order_relaxed);
    CHECK(after == before);
}

#else
    TEST_CASE("No malloc test skipped (no linker wrap support)") { CHECK(true); }
#endif