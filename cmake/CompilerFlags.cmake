# Copyright 2026 51ddhesh
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE or copy at
# http://www.boost.org/LICENSE_1_0.txt)

include(CheckCXXCompilerFlag)

function(velox_apply_compiler_flags tgt)
  if (MSVC)
    message(FATAL_ERROR "Velox currently targets GCC/Clang on Linux for low-latency features.")
  endif()

  # Warnings 
  target_compile_options(${tgt} PRIVATE
    -Wall -Wextra -Wpedantic
    -Wshadow -Wconversion -Wsign-conversion
    -Wdouble-promotion
    -Wnull-dereference
    -Wformat=2
  )

  # Core performance flags
  target_compile_options(${tgt} PRIVATE
    -fno-exceptions
    -fno-rtti
  )

  # Release tuning
  if (CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    target_compile_options(${tgt} PRIVATE -O3 -fomit-frame-pointer)

    if (VELOX_NATIVE_ARCH)
      target_compile_options(${tgt} PRIVATE -march=native)
    endif()

    if (VELOX_ENABLE_LTO)
      include(CheckIPOSupported)
      check_ipo_supported(RESULT ipo_ok OUTPUT ipo_err)
      if (ipo_ok)
        set_property(TARGET ${tgt} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
      else()
        message(WARNING "LTO requested but not supported: ${ipo_err}")
      endif()
    endif()
  endif()

  # PGO
  if (VELOX_PGO_MODE STREQUAL "GENERATE")
    target_compile_options(${tgt} PRIVATE -fprofile-generate)
    target_link_options(${tgt} PRIVATE -fprofile-generate)
  elseif (VELOX_PGO_MODE STREQUAL "USE")
    target_compile_options(${tgt} PRIVATE -fprofile-use -fprofile-correction)
    target_link_options(${tgt} PRIVATE -fprofile-use -fprofile-correction)
  endif()

endfunction()