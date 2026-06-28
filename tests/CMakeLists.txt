# Copyright 2026 51ddhesh
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE or copy at
# http://www.boost.org/LICENSE_1_0.txt)

velox_fetch_doctest()

add_executable(velox_tests
    test_smoke.cpp
)

target_link_libraries(velox_tests PRIVATE velox doctest::doctest_with_main)

add_test(NAME velox_tests COMMAND velox_tests)