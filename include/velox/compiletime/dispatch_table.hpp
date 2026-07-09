// Copyright 2026 51ddhesh
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once


#include <array>
#include <cstdint>

#include <velox/sbe/order_event.hpp>

// Forward Decalaration
namespace velox { struct SymbolTable; } 

namespace velox::compiletime {

using ParseFn = sbe::OrderEvent (*)(const uint8_t*, const SymbolTable&) noexcept;

// Unknown msg_types -> invalid events (cold path)
inline constexpr ParseFn null_parse = 
    +[](const uint8_t*, const SymbolTable&) noexcept -> sbe::OrderEvent {
        return {}; // deafult = invalid
    };

struct DispatchEntry {
    uint8_t msg_type;
    ParseFn fn;
};

template <std::size_t N>
consteval std::array<ParseFn, 256>
build_dispatch_table(const std::array<DispatchEntry, N>& entries) noexcept {
    std::array<ParseFn, 256> t{};

    for (auto& slot : t) slot = null_parse;

    for (std::size_t i = 0; i < N; i++) {
        t[entries[i].msg_type] = entries[i].fn;
    }

    return t;
}

} // namespace velox::compiletime

