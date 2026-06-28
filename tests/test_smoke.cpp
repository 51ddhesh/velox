// Copyright 2026 51ddhesh
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <velox/sbe/order_event.hpp>

TEST_CASE("OrderEvent is one cache line") {
    CHECK(sizeof(velox::sbe::OrderEvent) == 64);
    CHECK(alignof(velox::sbe::OrderEvent) == 64);
}