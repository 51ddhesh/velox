# Copyright 2026 51ddhesh
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE or copy at
# http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 4.2)
set(CMAKE_POLICY_VERSION_MINIMUM 3.25)

project(velox
    VERSION 0.1.0
    DESCRIPTION "Ultra Low Latency Orderbook"
    LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(VELOX_BUILD_TESTS "Build unit tests" ON)
option(VELOX_BUILD_BENCH "Build benchmarks" ON)
option(VELOX_BUILD_TOOLS "Build tools" ON)

option(VELOX_ENABLE_LTO  "Enable link-time optimization" ON)
option(VELOX_NATIVE_ARCH "Use -march=native in Release" ON)

# Policy: fail fast on hugepage/mlock failures etc.
option(VELOX_FAIL_FAST          "Abort on critical runtime setup failures" ON)

# Optional Linux-only dependencies
option(VELOX_WITH_URING         "Enable io_uring components when available" ON)

# PGO wiring (two-stage build).
set(VELOX_PGO_MODE "OFF" CACHE STRING "PGO mode: OFF | GENERATE | USE")
set_property(CACHE VELOX_PGO_MODE PROPERTY STRINGS OFF GENERATE USE)

add_library(velox STATIC
  src/velox.cpp
)

target_include_directories(velox
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(velox PUBLIC cxx_std_23)

include(cmake/CompilerFlags.cmake)
include(cmake/SIMDDetect.cmake)
include(cmake/Dependencies.cmake)

velox_apply_compiler_flags(velox)
velox_detect_simd(velox)
velox_configure_optional_deps(velox)

target_compile_definitions(velox PUBLIC
  $<$<BOOL:${VELOX_FAIL_FAST}>:VELOX_FAIL_FAST=1>
  $<$<NOT:$<BOOL:${VELOX_FAIL_FAST}>>:VELOX_FAIL_FAST=0>
)

if (VELOX_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

if (VELOX_BUILD_BENCH)
  add_subdirectory(bench)
endif()

if (VELOX_BUILD_TOOLS)
  add_subdirectory(tools)
endif()
