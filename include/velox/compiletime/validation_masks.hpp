// Copyright 2026 51ddhesh
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once


#include <array>
#include <cstdint>

namespace velox::compiletime {

inline constexpr std::array<uint16_t, 48> TAGS = {
    8, 9, 10, 11, 14, 17, 20, 21,
    35, 37, 38, 39, 40, 41, 44, 49,
    52, 54, 55, 56, 57, 58, 60, 99,
    100,150,151,167,198,207,231,375,
    11'000, 11'001, 11'002, 11'003, 11'004, 11'005, 11'006, 11'007,
    11'008, 11'009, 11'010, 11'011, 11'012, 11'013, 11'014, 11'015
};

consteval uint8_t tag_to_bit(uint16_t tag) noexcept {
    for (uint8_t i = 0; i < static_cast<uint8_t>(TAGS.size()); i++) {
        if (TAGS[i] == tag) return i;
    }

    // if unknown
    return 255;
}

template <uint16_t... Tags>
consteval uint64_t required_mask() noexcept {
    uint64_t mask = 0;
    // lambda fold
    (
        [&] {
            constexpr uint8_t bit = tag_to_bit(Tags);
            static_assert(bit != 255, "Unknown FIX tag in required_mask<>()");
            static_assert(bit < 64,  "required_mask requires bit < 64");
            mask |= (uint64_t{1} << bit);
        }(),
        ...
    );

    return mask;
}

[[nodiscard]] inline constexpr bool
validate_fix_fields(uint64_t seen_mask, uint64_t req_mask) noexcept {
    return (seen_mask & req_mask) == req_mask;
}

inline constexpr uint64_t NOS_REQUIRED_MASK = 
    required_mask<11, 21, 38, 40, 49, 54, 55, 60>();

inline constexpr uint64_t OCR_REQUIRED_MASK =
    required_mask<11, 37, 41, 49, 54, 55, 60>();

static_assert(NOS_REQUIRED_MASK != 0);
static_assert(OCR_REQUIRED_MASK != 0);

} // namespace velox::compiletime
