// Copyright 2026 51ddhesh
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>

#include <velox/compiletime/dispatch_table.hpp>
#include <velox/sbe/order_event.hpp>

namespace velox {

struct SybmolTable {};

}

static velox::sbe::OrderEvent parse_A(const uint8_t*, const velox::SymbolTable&) noexcept {
    velox::sbe::OrderEvent ev{};
    ev.event_type = 1;
    return ev;
}

static velox::sbe::OrderEvent parse_E(const uint8_t*, const velox::SymbolTable&) noexcept {
    velox::sbe::OrderEvent ev{};
    ev.event_type = 2;
    return ev;
}

TEST_CASE("consteval dispatch table builds and routes") {
    constexpr auto table = velox::compiletime::build_dispatch_table(
        std::array {
            velox::compiletime::DispatchEntry{static_cast<uint8_t>('A'), &parse_A},
            velox::compiletime::DispatchEntry{static_cast<uint8_t>('E'), &parse_E},
        }
    );

    static_assert(table[static_cast<uint8_t>('A')] == &parse_A);
    static_assert(table[static_cast<uint8_t>('E')] == &parse_E);
    static_assert(table[static_cast<uint8_t>('Z')] == velox::compiletime::null_parse);
}
