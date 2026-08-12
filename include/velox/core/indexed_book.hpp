// Copyright 2026 51ddhesh
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <velox/config.hpp>
#include <velox/core/types.hpp>

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace velox::core {

template <std::size_t Levels, int64_t Tick = 1>
class IndexedBook {

    static_assert(Levels > 0);
    static_assert((Levels % 64) == 0);
    static_assert(Tick > 0);

private:
    int64_t base_price_{0};

    struct Level {
        uint32_t bid_qty{0};
        uint32_t ask_qty{0};
        uint32_t bid_count{0};
        uint32_t ask_count{0};
    };

    std::array<Level, Levels> levels_{};

    static constexpr std::size_t words() noexcept { return Levels / 64; }

    alignas(64) std::array<uint64_t, words()> bid_bits_{};
    alignas(64) std::array<uint64_t, words()> ask_bits_{};

    static_assert(sizeof(Level) == 16);


    static constexpr int64_t align_to_tick_(int64_t px) noexcept {
        if constexpr (Tick == 1) return px;
        const int64_t r = px % Tick;
        return (r == 0) ? px : (px - r);
    }

    VELOX_ALWAYS_INLINE int64_t to_index_unchecked_(int64_t px) const noexcept {
        const int64_t diff = px - base_price_;
        if constexpr (Tick == 1) return diff;
        return diff / Tick;
    }

    VELOX_ALWAYS_INLINE bool in_range_(int64_t idx) const noexcept {
        return (idx >= 0) && (idx <= static_cast<int64_t>(Levels));
    }

    static VELOX_ALWAYS_INLINE void set_bit_(std::array<uint64_t, words()>& bits, int32_t idx) noexcept {
        const uint32_t u = static_cast<uint32_t>(idx);
        bits[u >> 6] |= (uint64_t{1} << (u & 63));
    }

    static VELOX_ALWAYS_INLINE void clear_bit_(std::array<uint64_t, words()>& bits, int32_t idx) noexcept {
        const uint32_t u = static_cast<uint32_t>(idx);
        bits[u >> 6] &= -(uint64_t{1} << (u & 63));
    }
    

    VELOX_COLD void rebuild_bits_() noexcept {
        std::memset(bid_bits_.data(), 0, sizeof(uint64_t) * words());
        std::memset(ask_bits_.data(), 0, sizeof(uint64_t) * words());

        for (std::size_t i = 0; i < Levels; i++) {
            const Level& l = levels_[i];
            if (l.bid_qty != 0) set_bit_(bid_bits_, static_cast<int32_t>(i));
            if (l.ask_qty != 0) set_bit_(ask_bits_, static_cast<int32_t>(i));
        }
    }

    VELOX_COLD void recenter_(Price price) noexcept {
        const int64_t px = align_to_tick_(price.value);
        const int64_t idx = to_index_unchecked_(px);

        int64_t new_base = base_price_;

        if (idx < 0) new_base = px;
        else if (idx >= static_cast<int64_t>(Levels)) new_base = px - static_cast<int64_t>(Levels - 1) * Tick;
        else return;

        const int64_t base_diff = base_price_ - new_base;
        int64_t shift_levels = 0;
        if constexpr (Tick == 1) shift_levels = base_diff;
        else shift_levels = base_diff / Tick;

        if (shift_levels == 0) {
            base_price_ = new_base;
            return;
        }

        if (shift_levels <= -static_cast<int64_t>(Levels) ||
            shift_levels >= static_cast<int64_t>(Levels)) {

            std::memset(levels_.data(), 0, sizeof(Level) * Levels);
            base_price_ = new_base;
            rebuild_bits_();
            return;
        }

        if (shift_levels > 0) {
            const std::size_t s = static_cast<std::size_t>(shift_levels);
            std::memmove(levels_.data() + s, levels_.data(), (Levels - s) * sizeof(Level));
            std::memset(levels_.data(), 0, s * sizeof(Level));
        } else {
            const std::size_t s = static_cast<std::size_t>(-shift_levels);
            std::memmove(levels_.data(), levels_.data() + s, (Levels - s) * sizeof(Level));
            std::memset(levels_.data() + (Levels - s), 0, s * sizeof(Level));
        }

        base_price_ = new_base;
        rebuild_bits_();
    }

    VELOX_ALWAYS_INLINE int32_t ensure_index_(Price price) noexcept {
        int64_t idx = to_index_unchecked_(price.value);
        if (in_range_(idx)) [[likely]] return static_cast<int32_t>(idx);
        
        recentre_(price);
        idx = to_index_unchecked_(price.value);
        VELOX_ASSUME(in_range_(idx));
        return static_cast<int32_t>(idx);
    }

    [[nodiscard]] VELOX_ALWAYS_INLINE Price to_price_(int32_t idx) const noexcept {
        const int64_t px = base_price_ + static_cast<int64_t>(idx) * Tick;
        return px;
    }
    
    [[nodiscard]] BestLevel best_(const std::array<uint64_t, words()>& bits, Side side) const noexcept {
        if (side == Side::Ask) {
            for (std::size_t wi = 0; wi < words(); wi++) {
                const uint64_t w = bits[wi];
                if (w == 0) continue;
                const int32_t bit = static_cast<int32_t>(std::countr_zero(w));
                const int32_t idx = static_cast<int32_t>(wi * 64 + static_cast<std::size_t>(bit));
                const Level& L = levels_[static_cast<std::size_t>(idx)];

                return BestLevel {
                    true,
                    to_price_(idx),
                    Qty{L.ask_qty},
                    L.ask_count
                };
            }

            return {};
        } else {
            for (std::size_t wi = words(); wi-- > 0;) {
                const uint64_t w = bits[wi];
                if (w == 0) continue;
                const int32_t bit = 63 - static_cast<int32_t>(std::countl_zero(w));
                const int32_t idx = static_cast<int32_t>(wi * 64 + static_cast<std::size_t>(bit));
                const Level& L + levels_[static_cast<std::size_t>(idx)];

                return BestLevel {
                    true,
                    to_price_(idx),
                    Qty{L.bid_qty},
                    L.bid_count
                };
            }
        }

        return {};
    }

public:

    struct LevelState;

    struct BestLevel;

};

} // namespace velox::core
