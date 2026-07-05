// Copyright 2026 51ddhesh
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once


#include <velox/config.hpp>
#include <velox/core/fatal.hpp>

#include <cstddef>
#include <cstdint>
#include <utility>

#include <sys/mman.h>
#include <unistd.h>

namespace velox::memory {

enum class HugePageMode : uint8_t {
    Disable = 0,
    Prefer = 1,
    Require = 2,
};

enum class LockMode : uint8_t {
    DontLock = 0,
    Lock = 1,
};

[[nodiscard]] constexpr std::size_t round_up(std::size_t n, std::size_t multiple) noexcept {
    return ((n + multiple - 1) / multiple) * multiple;  
}

[[nodiscard]] inline std::byte* align_up(std::byte* p, std::size_t alignment) noexcept {
    const auto v = reinterpret_cast<std::uintptr_t>(p);
    const auto a = static_cast<std::uintptr_t>(alignment);
    const auto aligned = (v + (a - 1)) & ~(a - 1);
    return reinterpret_cast<std::byte*>(aligned);
}

class HugePageMapping {
private:
    bool is_huge_{false};
    std::size_t size_{0};
    void* ptr_{nullptr};

    static constexpr std::size_t kHugePageSize = 2ull * 1024 * 1024;

    void map_(std::size_t bytes, HugePageMode huge_mode, LockMode lock_mode) noexcept {
        if (bytes == 0) {
            velox::core::fatal("HugePageMapping: bytes == 0");
        }

        // don't attempt MAP_HUGETLB for small mappings
        const bool attempt_huge = (huge_mode != HugePageMode::Disable) && (bytes >= kHugePageSize);

        // over-allocate to hugepage multiple only when attempting huge pages 
        const std::size_t map_size = attempt_huge ? round_up(bytes, kHugePageSize) : bytes;

        void* p = MAP_FAILED;

        #if defined(MAP_HUGETLB) 
            if (attempt_huge) {
                int flags = MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB;
                // request 2MB huge pages 
                #if defined(MAP_HUGE_2MB)
                    flags |= MAP_HUGE_2MB;
                #endif

                p = ::mmap(nullptr, map_size, PROT_READ | PROT_WRITE, flags, -1, 0);
            }
        #endif 

        if (p == MAP_FAILED) {
            if (attempt_huge && huge_mode == HugePageMode::Require) {
                velox::core::fatal_errno("mmap(MAP_HUGETLB) failed and huge pages are required");
            }

            p = ::mmap(nullptr, map_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

            if (p == MAP_FAILED) {
                velox::core::fatal_errno("mmap(MAP_ANONYMOUS) failed");
            }

            #if defined(MADV_HUGEPAGE) 
                (void)::madvise(p, map_size, MADV_HUGEPAGE);
            #endif
        } else {
            is_huge_ = true;
        }

        ptr_ = p;
        size_ = map_size;

        if (lock_mode == LockMode::Lock) {
            if (::mlock(ptr_, size_) != 0) {
                if (velox::kFailFast) {
                    velox::core::fatal_errno("mlock failed (fail-fast)");
                }
            }
        }
    }

    void unmap_() noexcept {
        if (ptr_) {
            (void)::munmap(ptr_, size_);
            ptr_ = nullptr;
            size_ = 0;
            is_huge_ = false;
        }
    }

public:
    HugePageMapping() = default;
    HugePageMapping(
        std::size_t bytes,
        HugePageMode huge_mode = HugePageMode::Prefer,
        LockMode lock_mode = LockMode::Lock
    ) noexcept {
        map_(bytes, huge_mode, lock_mode);
    }

    HugePageMapping(const HugePageMapping&) = delete;
    HugePageMapping& operator=(const HugePageMapping&) = delete;

    HugePageMapping(HugePageMapping&& o) noexcept {
        *this = std::move(o);
    }
    HugePageMapping& operator=(HugePageMapping&& o) noexcept {
        if (this == &o) return *this;
        unmap_();
        ptr_ = o.ptr_;
        size_ = o.size_;
        is_huge_ = o.is_huge_;
        o.ptr_ = nullptr;
        o.size_ = 0;
        o.is_huge_ = false;
        return *this;
    }

    ~HugePageMapping() noexcept { unmap_(); }

    [[nodiscard]] void* data() noexcept { return ptr_; }
    [[nodiscard]] const void* data() const noexcept { return ptr_; }
    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    [[nodiscard]] bool is_huge() const noexcept { return is_huge_; }
    [[nodiscard]] explicit operator bool() const noexcept { return ptr_ != nullptr; }
};

} // namespace velox::memory
