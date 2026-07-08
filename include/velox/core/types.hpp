// Copyright 2026 51ddhesh
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once


#include <compare>
#include <cstdint>
#include <type_traits>

namespace velox {

template <class Rep, class Tag>
struct Strong {
    using rep_type = Rep;
    Rep value{};

    constexpr Strong() noexcept = default;
    constexpr explicit Strong(Rep v) noexcept : value(v) {}

    friend constexpr auto operator<=>(Strong, Strong) noexcept = default;

    [[nodiscard]] constexpr Rep unwrap() const noexcept { return value; }
};

struct PriceTag {};
struct QtyTag {};
struct OrderIdTag {};
struct SymbolIdTag {};

using Price = Strong<int64_t, PriceTag>;    // fixed-point × 10000
using Qty = Strong<uint32_t, QtyTag>;
using OrderId = Strong<uint64_t, OrderIdTag>;
using SymbolId = Strong<uint16_t, SymbolIdTag>;

enum class Side : uint8_t {
    Bid = 0,
    Ask = 1
};

static_assert(std::is_trivially_copyable_v<Price>);
static_assert(std::is_trivially_copyable_v<Qty>);
static_assert(std::is_trivially_copyable_v<OrderId>);
static_assert(std::is_trivially_copyable_v<SymbolId>);

static_assert(sizeof(Price) == sizeof(int64_t));
static_assert(sizeof(Qty) == sizeof(uint32_t));
static_assert(sizeof(OrderId) == sizeof(uint64_t));
static_assert(sizeof(SymbolId) == sizeof(uint16_t));

} // namespace velox
