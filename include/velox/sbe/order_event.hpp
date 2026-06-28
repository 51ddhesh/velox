// Copyright 2026 51ddhesh
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once


#include <cstddef>
#include <cstdint>

namespace velox::sbe {

enum class EventType : uint8_t {
    Add = 0,
    Cancel = 1,
    Execute = 2,
    Delete = 3,
    Replace = 4,
    Invalid = 0xFF,
};

struct alignas(64) OrderEvent {
    int64_t price{};
    uint64_t timestamp_ns{};
    uint64_t order_id{};
    uint64_t replace_order_id{};
    uint32_t quantity{};
    uint32_t repace_qty{};
    uint16_t symbol_id{};
    uint8_t event_type{static_cast<uint8_t>(EventType::Invalid)};
    uint8_t side{};
    uint8_t _pad[20]{};
};

static_assert(sizeof(OrderEvent) == 64);
static_assert(alignof(OrderEvent) == 64);
static_assert(offsetof(OrderEvent, price) == 0);
static_assert(offsetof(OrderEvent, timestamp_ns) == 8);
static_assert(offsetof(OrderEvent, order_id) == 16);
static_assert(offsetof(OrderEvent, replace_order_id) == 24);
static_assert(offsetof(OrderEvent, quantity) == 32);
static_assert(offsetof(OrderEvent, repace_qty) == 36);
static_assert(offsetof(OrderEvent, symbol_id) == 40);
static_assert(offsetof(OrderEvent, event_type) == 42);
static_assert(offsetof(OrderEvent, side) == 43);
static_assert(offsetof(OrderEvent, _pad) == 44);

[[nodiscard]] constexpr bool is_valid(const OrderEvent& ev) noexcept {
    return ev.event_type != static_cast<uint8_t>(EventType::Invalid);
}

} // namespace velox::sbe
