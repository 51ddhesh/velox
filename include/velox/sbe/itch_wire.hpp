// Copyright 2026 51ddhesh
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once


#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace velox::sbe::itch {

#pragma pack(push, 1)
struct AddOrder {
    char msg_type;
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t order_ref_num;
    char buy_sell;
    uint32_t shares;
    char stock[8];
    uint32_t price; // times 10000
};

static_assert(sizeof(AddOrder) == 36);
static_assert(offsetof(AddOrder, order_ref_num) == 11);
static_assert(offsetof(AddOrder, shares) == 20);
static_assert(offsetof(AddOrder, price) == 32);

struct Execute {
    char msg_type;
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t order_ref_num;
    uint32_t executed_shares;
    uint64_t match_number;
};

static_assert(sizeof(Execute) == 31);
static_assert(offsetof(Execute, executed_shares) == 19);

struct Cancel {
    char msg_type;
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t order_ref_num;
    uint32_t cancelled_shares;
};

static_assert(sizeof(Cancel) == 23);

struct Delete {
    char msg_type;
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t order_ref_num;
};

static_assert(sizeof(Delete) == 19);

struct Replace {
    char msg_type;
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t orig_order_ref_num;
    uint64_t new_order_ref_num;
    uint32_t shares;
    uint32_t price;
};

static_assert(sizeof(Replace) == 35);
static_assert(offsetof(Replace, orig_order_ref_num) == 11);
static_assert(offsetof(Replace, new_order_ref_num) == 19);
static_assert(offsetof(Replace, shares) == 27);
static_assert(offsetof(Replace, price) == 31);

struct StockDirectory {
    char msg_type;
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    char stock[8];
    char market_category;
    char financial_status;
    uint32_t round_lot_size;
    char round_lots_only;
    char issue_classification;
    char issue_sub_type[2];
    char authenticity;
    char short_sale_threshold;
    char ipo_flag;
    char luld_ref_price_tier;
    char etp_flag;
    uint32_t etp_leverage_factor;
    char inverse_indicator;
};

static_assert(sizeof(StockDirectory) == 39);
static_assert(offsetof(StockDirectory, stock) == 11);
static_assert(offsetof(StockDirectory, round_lot_size) == 21);

#pragma pack(pop)

[[nodiscard]] inline uint16_t u16(const void* p) noexcept {
    uint16_t v;
    std::memcpy(&v, p, sizeof(v));
    return std::byteswap(v);
}

[[nodiscard]] inline uint32_t u32(const void* p) noexcept {
    uint32_t v;
    std::memcpy(&v, p, sizeof(v));
    return std::byteswap(v);
}

[[nodiscard]] inline uint64_t u64(const void* p) noexcept {
    uint64_t v;
    std::memcpy(&v, p, sizeof(v));
    return std::byteswap(v);
}

[[nodiscard]] inline uint64_t u48(const uint8_t* p) noexcept {
    return (uint64_t{p[0]} << 40) | (uint64_t{p[1]} << 32) |
           (uint64_t{p[2]} << 24) | (uint64_t{p[3]} << 16) |
            (uint64_t{p[4]} << 8) | uint64_t{p[5]};
}


} // namespace velox::sbe::itch
