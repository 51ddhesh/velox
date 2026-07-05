// Copyright 2026 51ddhesh
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once


#include <velox/memory/huge_page.hpp>

#include <cstddef>
#include <cstdint> 
#include <new>

namespace velox::memory {

class Arena {
private:
    HugePageMapping mapping_{};
    std::byte* base_{nullptr};
    std::size_t cap_{0};
    std::size_t off_{0};

public:
    Arena() = default;

    explicit Arena(
        std::size_t bytes,
        HugePageMode huge_mode = HugePageMode::Prefer,
        LockMode lock_mode = LockMode::Lock
    ) noexcept :
        mapping_(bytes, huge_mode, lock_mode), 
        base_(static_cast<std::byte*>(mapping_.data())),
        cap_(mapping_.size()),
        off_{0} {}


    Arena(const Arena&) = delete;
    Arena& operator=(const Arena&) = delete;

    [[nodiscard]] std::size_t capacity() const noexcept { return cap_; }
    [[nodiscard]] std::size_t used() const noexcept { return off_; }
    [[nodiscard]] std::size_t remaining() const noexcept { return cap_ - off_; }

    void reset() noexcept { off_ = 0; }

    [[nodiscard]] void* allocate_bytes(
        std::size_t bytes,
        std::size_t alignment = alignof(std::max_align_t)
    ) noexcept {
        if (bytes == 0) return nullptr;

        if (alignment & (alignment - 1) != 0) return nullptr;

        std::byte* p = base_ + off_;
        p = align_up(p, alignment);

        const std::size_t new_off = static_cast<std::size_t>(p - base_) + bytes;
        if (new_off > cap_) return nullptr;

        off_ = new_off;

        return p;
    }

    template <class T>
    [[nodiscard]] T* allocate(std::size_t count = 1) noexcept {
        static_assert(std::is_trivially_destructible_v<T>);
        void* p = allocate_bytes(sizeof(T) * count, alignof(T));

        return static_cast<T*>(p);
    }
};

} // namespace velox::memory