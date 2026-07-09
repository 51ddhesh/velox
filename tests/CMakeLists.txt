# Copyright 2026 51ddhesh
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE or copy at
# http://www.boost.org/LICENSE_1_0.txt)

velox_fetch_doctest()

add_executable(velox_tests
    test_smoke.cpp
    test_huge_page.cpp
    test_arena.cpp
    test_fixed_slot_allocator.cpp
    test_no_malloc.cpp

    test_dispatch_table.cpp
    test_validation_masks.cpp
    test_routing_keys.cpp
)

target_link_libraries(velox_tests PRIVATE velox doctest::doctest_with_main)

if (UNIX AND NOT APPLE)
    target_link_options(velox_tests PRIVATE
        -Wl,--wrap=malloc
        -Wl,--wrap=free
    )
    target_compile_definitions(velox_tests PRIVATE VELOX_HAVE_MALLOC_WRAP=1)
else()
    target_compile_definitions(velox_tests PRIVATE VELOX_HAVE_MALLOC_WRAP=0)
endif()

add_test(NAME velox_tests COMMAND velox_tests)