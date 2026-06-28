# Copyright 2026 51ddhesh
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE or copy at
# http://www.boost.org/LICENSE_1_0.txt)

include(FetchContent)

function(velox_configure_optional_deps tgt)
  # io_uring (Linux only, optional)
  if (VELOX_WITH_URING AND UNIX AND NOT APPLE)
    find_library(URING_LIB uring)
    find_path(URING_INCLUDE liburing.h)

    if (URING_LIB AND URING_INCLUDE)
      add_library(velox_uring INTERFACE)
      target_include_directories(velox_uring INTERFACE ${URING_INCLUDE})
      target_link_libraries(velox_uring INTERFACE ${URING_LIB})
      target_compile_definitions(velox_uring INTERFACE VELOX_HAVE_URING=1)

      target_link_libraries(${tgt} PUBLIC velox_uring)
    else()
      message(STATUS "liburing not found; building without io_uring components.")
      target_compile_definitions(${tgt} PUBLIC VELOX_HAVE_URING=0)
    endif()
  else()
    target_compile_definitions(${tgt} PUBLIC VELOX_HAVE_URING=0)
  endif()
endfunction()

# ---- Test dependency (doctest, header-only) ----
function(velox_fetch_doctest)
  FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG        v2.4.11
  )
  FetchContent_MakeAvailable(doctest)
endfunction()

# ---- Benchmark dependency (google/benchmark) ----
function(velox_fetch_benchmark)
  set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
  set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
  FetchContent_Declare(
    benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG        v1.9.0
  )
  FetchContent_MakeAvailable(benchmark)
endfunction()