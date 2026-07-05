// Copyright 2026 51ddhesh
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once


#include <velox/config.hpp>
#include <velox/core/fatal.hpp>
#include <velox/memory/huge_page.hpp>

#include <cstddef>
#include <cstdint> 
#include <type_traits>

namespace velox::memory {

// single-thread-owned fixed size allocator
// backing store: one mmap region containing [slots][padding][free_stack]
template <typename T, std::size_t Capacity>
class FixedSizeSlotAllocator {
private:
    HugePageMapping mapping_{};
    T* slots_{nullptr};
    uint32_t* free_stack_{nullptr};
    std::size_t top_{0};

    void init_(HugePageMode huge_mode, LockMode lock_mode) noexcept {
        constexpr std::size_t kAlign = 64;
        const std::size_t slots_bytes = sizeof(T) * Capacity;
        const std::size_t stack_bytes = sizeof(uint32_t) * Capacity;

        // Include padding to align free_stack_ to 64 bytes
        const std::size_t total_bytes = slots_bytes + stack_bytes + kAlign;
        
        mapping_ = HugePageMapping(total_bytes, huge_mode, lock_mode);

        auto* base = static_cast<std::byte*>(mapping_.data());
        slots_ = reinterpret_cast<T*>(base);

        std::byte* p = base + slots_bytes;
        p = align_up(p, kAlign);
        free_stack_ = reinterpret_cast<uint32_t*>(p);

        // LIFO init
        for (uint32_t i = 0; i < Capacity; i++) {
            free_stack_[i] = static_cast<uint32_t>(Capacity - 1 - i);
        }

        top_ = Capacity;
    }

    [[noreturn]] VELOX_COLD void on_exhaustion_() noexcept {
        if (velox::kFailFast) {
            velox::core::fatal("[FixedSizeAllocator]: exhausted (fail-fast)");
        }

        velox::core::fatal("[FixedSizeAllocator]: exhausted");
    }

public:
    static_assert(Capacity > 0);
    static_assert(std::is_trivially_destructible_v<T>);

    FixedSizeSlotAllocator(
        HugePageMode huge_mode = HugePageMode::Prefer,
        LockMode lock_mode = LockMode::Lock
    ) noexcept {
        init_(huge_mode, lock_mode);
    }

    FixedSizeSlotAllocator(const FixedSizeSlotAllocator&) = delete;
    FixedSizeSlotAllocator& operator=(const FixedSizeSlotAllocator&) = delete;

    [[nodiscard]] constexpr std::size_t capacity() const noexcept { return Capacity; }
    [[nodiscard]] std::size_t available() const noexcept { return top_; }

    [[nodiscard]] VELOX_ALWAYS_INLINE T* allocate() noexcept {
        if (top_ == 0) [[unlikely]] {
            on_exhaustion_();
            return nullptr;
        }

        const uint32_t idx = free_stack_[--top_];
        return slots_ + idx;
    }

    VELOX_ALWAYS_INLINE void deallocate(T* ptr) noexcept {
        VELOX_ASSUME(ptr != nullptr);
        VELOX_ASSUME(ptr >= slots_ && ptr < (slots_ + Capacity));

        const uint32_t idx = static_cast<uint32_t>(ptr - slots_);
        free_stack_[top_++] = idx;
    }
    
    [[nodiscard]] bool owns(const T* ptr) const noexcept {
        return ptr >= slots_ && ptr < (slots_ + Capacity);
    }
};

} // namespace velox::memory
