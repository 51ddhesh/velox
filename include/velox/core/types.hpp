// Copyright 2026 51ddhesh
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once


#include <cstdint>
#include <type_traits>

namespace velox {

struct Price { int64_t value; };
struct Qty { uint32_t value; };
struct OrderID { uint64_t value; };
struct SymbolID { uint16_t value; };

enum class Side : uint8_t {
    Bid = 0,
    Ask = 1
};

static_assert(std::is_trivially_copyable_v<Price>);
static_assert(std::is_trivially_copyable_v<Qty>);
static_assert(std::is_trivially_copyable_v<OrderID>);
static_assert(std::is_trivially_copyable_v<SymbolID>);

} // namespace velox
