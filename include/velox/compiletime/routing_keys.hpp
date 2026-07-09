// Copyright 2026 51ddhesh
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once


#include <array>
#include <cstddef>
#include <string_view>

#include <velox/core/types.hpp>

namespace velox::compiletime {

template <std::size_t N>
struct SymbolRouter {
    std::array<std::string_view, N> symbols{};

    consteval SymbolId id_of(std::string_view sym) const noexcept {
        for (std::size_t i = 0; i < N; i++) {
            if (symbols[i] == sym) {
                return SymbolId{static_cast<uint16_t>(i)};
            }
        }

        return velox::kInvalidSymbol;
    }

    consteval bool contains(std::string_view sym) const noexcept {
        return id_of(sym).value != velox::kInvalidSymbol.value;
    }
};

} // namespace velox::compiletime
