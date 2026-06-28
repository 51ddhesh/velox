# Copyright 2026 51ddhesh
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE or copy at
# http://www.boost.org/LICENSE_1_0.txt)

include(CheckCXXCompilerFlag)

function(velox_detect_simd tgt)
  # Default: scalar
  target_compile_definitions(${tgt} PUBLIC VELOX_SIMD_SCALAR=1)

  check_cxx_compiler_flag("-mavx2" VELOX_HAS_AVX2)
  if (VELOX_HAS_AVX2)
    target_compile_options(${tgt} PRIVATE -mavx2)
    target_compile_definitions(${tgt} PUBLIC VELOX_SIMD_AVX2=1)
  endif()

  check_cxx_compiler_flag("-mavx512f" VELOX_HAS_AVX512F)
  if (VELOX_HAS_AVX512F)
    target_compile_options(${tgt} PRIVATE -mavx512f)
    target_compile_definitions(${tgt} PUBLIC VELOX_SIMD_AVX512=1)
  endif()
endfunction()