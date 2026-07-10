// Copyright 2026 51ddhesh
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <array>
#include <cstdint>
#include <cstring>

#include <velox/sbe/itch_wire.hpp>

static void put_u16_be(uint8_t* p, uint16_t v) {
    p[0] = static_cast<uint8_t>((v >> 8) & 0xFF);
    p[1] = static_cast<uint8_t>(v & 0xFF);
}
static void put_u32_be(uint8_t* p, uint32_t v) {
    p[0] = static_cast<uint8_t>((v >> 24) & 0xFF);
    p[1] = static_cast<uint8_t>((v >> 16) & 0xFF);
    p[2] = static_cast<uint8_t>((v >> 8) & 0xFF);
    p[3] = static_cast<uint8_t>(v & 0xFF);
}
static void put_u64_be(uint8_t* p, uint64_t v) {
    for (int i = 0; i < 8; ++i) {
        p[i] = static_cast<uint8_t>((v >> (56 - 8*i)) & 0xFF);
    }
}

TEST_CASE("ITCH AddOrder wire decode by offset") {
    using namespace velox::sbe::itch;

    std::array<uint8_t, sizeof(AddOrder)> buf{};
    buf[0] = 'A';
    put_u16_be(buf.data() + offsetof(AddOrder, stock_locate), 0x1234);
    put_u16_be(buf.data() + offsetof(AddOrder, tracking_number), 0x5678);

    // timestamp bytes at offset 5
    const std::size_t ts_off = offsetof(AddOrder, timestamp);
    buf[ts_off + 0] = 1;
    buf[ts_off + 1] = 2;
    buf[ts_off + 2] = 3;
    buf[ts_off + 3] = 4;
    buf[ts_off + 4] = 5;
    buf[ts_off + 5] = 6;

    put_u64_be(buf.data() + offsetof(AddOrder, order_ref_num), 0x1122334455667788ULL);
    buf[offsetof(AddOrder, buy_sell)] = 'B';
    put_u32_be(buf.data() + offsetof(AddOrder, shares), 500);
    std::memcpy(buf.data() + offsetof(AddOrder, stock), "AAPL    ", 8);
    put_u32_be(buf.data() + offsetof(AddOrder, price), 1'000'000); // 100.0000

    CHECK(buf[0] == 'A');
    CHECK(u16(buf.data() + offsetof(AddOrder, stock_locate)) == 0x1234);
    CHECK(u16(buf.data() + offsetof(AddOrder, tracking_number)) == 0x5678);
    CHECK(u48(buf.data() + offsetof(AddOrder, timestamp)) == 0x010203040506ULL);
    CHECK(u64(buf.data() + offsetof(AddOrder, order_ref_num)) == 0x1122334455667788ULL);
    CHECK(buf[offsetof(AddOrder, buy_sell)] == 'B');
    CHECK(u32(buf.data() + offsetof(AddOrder, shares)) == 500);
    CHECK(std::memcmp(buf.data() + offsetof(AddOrder, stock), "AAPL    ", 8) == 0);
    CHECK(u32(buf.data() + offsetof(AddOrder, price)) == 1'000'000);
}

TEST_CASE("ITCH Execute wire decode by offset") {
    using namespace velox::sbe::itch;

    std::array<uint8_t, sizeof(Execute)> buf{};
    buf[0] = 'E';
    put_u16_be(buf.data() + offsetof(Execute, stock_locate), 7);
    put_u64_be(buf.data() + offsetof(Execute, order_ref_num), 99);
    put_u32_be(buf.data() + offsetof(Execute, executed_shares), 1234);
    put_u64_be(buf.data() + offsetof(Execute, match_number), 0xDEADBEEFCAFEBABEULL);

    CHECK(u16(buf.data() + offsetof(Execute, stock_locate)) == 7);
    CHECK(u64(buf.data() + offsetof(Execute, order_ref_num)) == 99);
    CHECK(u32(buf.data() + offsetof(Execute, executed_shares)) == 1234);
    CHECK(u64(buf.data() + offsetof(Execute, match_number)) == 0xDEADBEEFCAFEBABEULL);
}