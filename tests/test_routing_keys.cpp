// Copyright 2026 51ddhesh
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <velox/compiletime/routing_keys.hpp>

TEST_CASE("SymbolRouter consteval ids") {
    using velox::compiletime::SymbolRouter;

    constexpr SymbolRouter<3> r{{
        "AAPL    ",
        "MSFT    ",
        "NVDA    "
    }};

    constexpr auto aapl = r.id_of("AAPL    ");
    constexpr auto bad  = r.id_of("BADSYM  ");

    static_assert(aapl.value == 0);
    static_assert(r.contains("MSFT    "));
    static_assert(bad.value == velox::kInvalidSymbol.value);
}