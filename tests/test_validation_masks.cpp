// Copyright 2026 51ddhesh
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <velox/compiletime/validation_masks.hpp>

TEST_CASE("tag_to_bit mapping is stable for known tags") {
    using namespace velox::compiletime;
    static_assert(tag_to_bit(11) != 255);
    static_assert(tag_to_bit(60) != 255);
    static_assert(tag_to_bit(9999) == 255);
}

TEST_CASE("required_mask + validate_fix_fields") {
    using namespace velox::compiletime;

    constexpr auto m = required_mask<11, 21, 38>();
    CHECK(m != 0);

    const uint64_t seen_ok = m;
    const uint64_t seen_missing = m & ~(uint64_t{1} << tag_to_bit(21));

    CHECK(validate_fix_fields(seen_ok, m));
    CHECK(!validate_fix_fields(seen_missing, m));
}

TEST_CASE("precomputed masks exist") {
    using namespace velox::compiletime;
    CHECK(NOS_REQUIRED_MASK != 0);
    CHECK(OCR_REQUIRED_MASK != 0);
}
