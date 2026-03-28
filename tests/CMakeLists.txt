# SPDX-FileCopyrightText: 2019–2026 Andy Curtis <contactandyc@gmail.com>
# SPDX-FileCopyrightText: 2024–2025 Knode.ai
# SPDX-License-Identifier: Apache-2.0
#
# Maintainer: Andy Curtis <contactandyc@gmail.com>

# CMakeLists.txt for tests
cmake_minimum_required(VERSION 3.20)

project(the_macro_library_tests LANGUAGES C)

set(A_BUILD_VARIANT "debug" CACHE STRING
    "Variant to link via the_macro_library::the_macro_library (debug|memory|static|shared)")
set_property(CACHE A_BUILD_VARIANT PROPERTY STRINGS debug memory static shared)

option(A_ENABLE_COVERAGE "Enable code coverage instrumentation" OFF)

find_library(M_LIB m)


# Fallback for standalone test builds (when not included via add_subdirectory)
if(NOT TARGET the_macro_library::the_macro_library)
  find_package(the_macro_library CONFIG REQUIRED)
endif()

set(TEST_EXECUTABLES "")

# ---- Targets ----
add_executable(test_macro_bsearch
  src/test_macro_bsearch.c
)

target_include_directories(test_macro_bsearch PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

list(APPEND TEST_EXECUTABLES test_macro_bsearch)

set_target_properties(test_macro_bsearch PROPERTIES
  C_STANDARD 23
  C_STANDARD_REQUIRED YES
)
if("CXX" IN_LIST CMAKE_PROJECT_LANGUAGES)
  set_target_properties(test_macro_bsearch PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
  )
endif()

target_link_libraries(test_macro_bsearch PRIVATE the_macro_library::the_macro_library)

if(M_LIB)
  target_link_libraries(test_macro_bsearch PRIVATE ${M_LIB})
endif()

if(MSVC)
  target_compile_options(test_macro_bsearch PRIVATE /W4)
else()
  target_compile_options(test_macro_bsearch PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(A_ENABLE_COVERAGE)
  if (CMAKE_C_COMPILER_ID MATCHES "Clang")
    target_compile_options(test_macro_bsearch PRIVATE -O0 -g -fprofile-instr-generate -fcoverage-mapping)
    target_link_options(test_macro_bsearch PRIVATE -fprofile-instr-generate -fcoverage-mapping)
  elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    target_compile_options(test_macro_bsearch PRIVATE -O0 -g --coverage)
    target_link_options(test_macro_bsearch PRIVATE --coverage)
  endif()
endif()

add_test(NAME test_macro_bsearch COMMAND $<TARGET_FILE:test_macro_bsearch>)
add_executable(test_macro_cmp
  src/test_macro_cmp.c
)

target_include_directories(test_macro_cmp PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

list(APPEND TEST_EXECUTABLES test_macro_cmp)

set_target_properties(test_macro_cmp PROPERTIES
  C_STANDARD 23
  C_STANDARD_REQUIRED YES
)
if("CXX" IN_LIST CMAKE_PROJECT_LANGUAGES)
  set_target_properties(test_macro_cmp PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
  )
endif()

target_link_libraries(test_macro_cmp PRIVATE the_macro_library::the_macro_library)

if(M_LIB)
  target_link_libraries(test_macro_cmp PRIVATE ${M_LIB})
endif()

if(MSVC)
  target_compile_options(test_macro_cmp PRIVATE /W4)
else()
  target_compile_options(test_macro_cmp PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(A_ENABLE_COVERAGE)
  if (CMAKE_C_COMPILER_ID MATCHES "Clang")
    target_compile_options(test_macro_cmp PRIVATE -O0 -g -fprofile-instr-generate -fcoverage-mapping)
    target_link_options(test_macro_cmp PRIVATE -fprofile-instr-generate -fcoverage-mapping)
  elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    target_compile_options(test_macro_cmp PRIVATE -O0 -g --coverage)
    target_link_options(test_macro_cmp PRIVATE --coverage)
  endif()
endif()

add_test(NAME test_macro_cmp COMMAND $<TARGET_FILE:test_macro_cmp>)
add_executable(test_macro_heap
  src/test_macro_heap.c
)

target_include_directories(test_macro_heap PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

list(APPEND TEST_EXECUTABLES test_macro_heap)

set_target_properties(test_macro_heap PROPERTIES
  C_STANDARD 23
  C_STANDARD_REQUIRED YES
)
if("CXX" IN_LIST CMAKE_PROJECT_LANGUAGES)
  set_target_properties(test_macro_heap PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
  )
endif()

target_link_libraries(test_macro_heap PRIVATE the_macro_library::the_macro_library)

if(M_LIB)
  target_link_libraries(test_macro_heap PRIVATE ${M_LIB})
endif()

if(MSVC)
  target_compile_options(test_macro_heap PRIVATE /W4)
else()
  target_compile_options(test_macro_heap PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(A_ENABLE_COVERAGE)
  if (CMAKE_C_COMPILER_ID MATCHES "Clang")
    target_compile_options(test_macro_heap PRIVATE -O0 -g -fprofile-instr-generate -fcoverage-mapping)
    target_link_options(test_macro_heap PRIVATE -fprofile-instr-generate -fcoverage-mapping)
  elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    target_compile_options(test_macro_heap PRIVATE -O0 -g --coverage)
    target_link_options(test_macro_heap PRIVATE --coverage)
  endif()
endif()

add_test(NAME test_macro_heap COMMAND $<TARGET_FILE:test_macro_heap>)
add_executable(test_macro_map
  src/test_macro_map.c
)

target_include_directories(test_macro_map PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

list(APPEND TEST_EXECUTABLES test_macro_map)

set_target_properties(test_macro_map PROPERTIES
  C_STANDARD 23
  C_STANDARD_REQUIRED YES
)
if("CXX" IN_LIST CMAKE_PROJECT_LANGUAGES)
  set_target_properties(test_macro_map PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
  )
endif()

target_link_libraries(test_macro_map PRIVATE the_macro_library::the_macro_library)

if(M_LIB)
  target_link_libraries(test_macro_map PRIVATE ${M_LIB})
endif()

if(MSVC)
  target_compile_options(test_macro_map PRIVATE /W4)
else()
  target_compile_options(test_macro_map PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(A_ENABLE_COVERAGE)
  if (CMAKE_C_COMPILER_ID MATCHES "Clang")
    target_compile_options(test_macro_map PRIVATE -O0 -g -fprofile-instr-generate -fcoverage-mapping)
    target_link_options(test_macro_map PRIVATE -fprofile-instr-generate -fcoverage-mapping)
  elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    target_compile_options(test_macro_map PRIVATE -O0 -g --coverage)
    target_link_options(test_macro_map PRIVATE --coverage)
  endif()
endif()

add_test(NAME test_macro_map COMMAND $<TARGET_FILE:test_macro_map>)
add_executable(test_macro_sort
  src/test_macro_sort.c
)

target_include_directories(test_macro_sort PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

list(APPEND TEST_EXECUTABLES test_macro_sort)

set_target_properties(test_macro_sort PROPERTIES
  C_STANDARD 23
  C_STANDARD_REQUIRED YES
)
if("CXX" IN_LIST CMAKE_PROJECT_LANGUAGES)
  set_target_properties(test_macro_sort PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
  )
endif()

target_link_libraries(test_macro_sort PRIVATE the_macro_library::the_macro_library)

if(M_LIB)
  target_link_libraries(test_macro_sort PRIVATE ${M_LIB})
endif()

if(MSVC)
  target_compile_options(test_macro_sort PRIVATE /W4)
else()
  target_compile_options(test_macro_sort PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(A_ENABLE_COVERAGE)
  if (CMAKE_C_COMPILER_ID MATCHES "Clang")
    target_compile_options(test_macro_sort PRIVATE -O0 -g -fprofile-instr-generate -fcoverage-mapping)
    target_link_options(test_macro_sort PRIVATE -fprofile-instr-generate -fcoverage-mapping)
  elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    target_compile_options(test_macro_sort PRIVATE -O0 -g --coverage)
    target_link_options(test_macro_sort PRIVATE --coverage)
  endif()
endif()

add_test(NAME test_macro_sort COMMAND $<TARGET_FILE:test_macro_sort>)
add_executable(test_macro_time
  src/test_macro_time.c
)

target_include_directories(test_macro_time PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

list(APPEND TEST_EXECUTABLES test_macro_time)

set_target_properties(test_macro_time PROPERTIES
  C_STANDARD 23
  C_STANDARD_REQUIRED YES
)
if("CXX" IN_LIST CMAKE_PROJECT_LANGUAGES)
  set_target_properties(test_macro_time PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
  )
endif()

target_link_libraries(test_macro_time PRIVATE the_macro_library::the_macro_library)

if(M_LIB)
  target_link_libraries(test_macro_time PRIVATE ${M_LIB})
endif()

if(MSVC)
  target_compile_options(test_macro_time PRIVATE /W4)
else()
  target_compile_options(test_macro_time PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(A_ENABLE_COVERAGE)
  if (CMAKE_C_COMPILER_ID MATCHES "Clang")
    target_compile_options(test_macro_time PRIVATE -O0 -g -fprofile-instr-generate -fcoverage-mapping)
    target_link_options(test_macro_time PRIVATE -fprofile-instr-generate -fcoverage-mapping)
  elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    target_compile_options(test_macro_time PRIVATE -O0 -g --coverage)
    target_link_options(test_macro_time PRIVATE --coverage)
  endif()
endif()

add_test(NAME test_macro_time COMMAND $<TARGET_FILE:test_macro_time>)
add_executable(test_macro_to
  src/test_macro_to.c
)

target_include_directories(test_macro_to PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

list(APPEND TEST_EXECUTABLES test_macro_to)

set_target_properties(test_macro_to PROPERTIES
  C_STANDARD 23
  C_STANDARD_REQUIRED YES
)
if("CXX" IN_LIST CMAKE_PROJECT_LANGUAGES)
  set_target_properties(test_macro_to PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
  )
endif()

target_link_libraries(test_macro_to PRIVATE the_macro_library::the_macro_library)

if(M_LIB)
  target_link_libraries(test_macro_to PRIVATE ${M_LIB})
endif()

if(MSVC)
  target_compile_options(test_macro_to PRIVATE /W4)
else()
  target_compile_options(test_macro_to PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(A_ENABLE_COVERAGE)
  if (CMAKE_C_COMPILER_ID MATCHES "Clang")
    target_compile_options(test_macro_to PRIVATE -O0 -g -fprofile-instr-generate -fcoverage-mapping)
    target_link_options(test_macro_to PRIVATE -fprofile-instr-generate -fcoverage-mapping)
  elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    target_compile_options(test_macro_to PRIVATE -O0 -g --coverage)
    target_link_options(test_macro_to PRIVATE --coverage)
  endif()
endif()

add_test(NAME test_macro_to COMMAND $<TARGET_FILE:test_macro_to>)

enable_testing()

# ---- Coverage aggregation ----
add_custom_target(coverage_report COMMENT "Generate coverage report")

if(A_ENABLE_COVERAGE AND CMAKE_C_COMPILER_ID MATCHES "Clang")
  include(ProcessorCount)
  ProcessorCount(NPROC)
  if(NPROC EQUAL 0)
    set(NPROC 4)
  endif()

  add_custom_command(TARGET coverage_report PRE_BUILD
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -j ${NPROC}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running tests with coverage instrumentation"
  )

  find_program(LLVM_PROFDATA_EXECUTABLE llvm-profdata)
  find_program(LLVM_COV_EXECUTABLE llvm-cov)
  if(LLVM_PROFDATA_EXECUTABLE AND LLVM_COV_EXECUTABLE)
    add_custom_command(TARGET coverage_report POST_BUILD
      COMMAND ${LLVM_PROFDATA_EXECUTABLE} merge -sparse *.profraw -o default.profdata
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Merging coverage data"
    )

    # For header-only or app stacks, report coverage from all test executables
    foreach(tgt IN LISTS TEST_EXECUTABLES)
      add_custom_command(TARGET coverage_report POST_BUILD
        COMMAND ${LLVM_COV_EXECUTABLE} show
                $<TARGET_FILE:${tgt}>
                -instr-profile=default.profdata
                -format=html -output-dir=coverage_html
                --ignore-filename-regex='/usr/.*'
                --show-regions --show-line-counts-or-regions
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generated LLVM coverage report for ${tgt} → open coverage_html/index.html"
      )
    endforeach()
  endif()
endif()
