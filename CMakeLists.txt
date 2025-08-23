# SPDX-FileCopyrightText: 2019–2025 Andy Curtis <contactandyc@gmail.com>
# SPDX-FileCopyrightText: 2024–2025 Knode.ai — technical questions: contact Andy (above)
# SPDX-License-Identifier: Apache-2.0

# CMakeLists.txt for libraries (regular or header-only)
cmake_minimum_required(VERSION 3.20)

project(the_macro_library
  VERSION 0.0.1
  LANGUAGES C
)
# ── Header-only target ─────────────────────────────────────────────────────────
add_library(the_macro_library INTERFACE)
add_library(the_macro_library::the_macro_library ALIAS the_macro_library)

target_include_directories(the_macro_library INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Optional: expose required features
target_compile_features(the_macro_library INTERFACE
  c_std_23
)

include(GNUInstallDirs)
install(DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Export + install targets
install(TARGETS the_macro_library EXPORT the_macro_libraryTargets)
install(EXPORT the_macro_libraryTargets
  FILE the_macro_libraryTargets.cmake
  NAMESPACE the_macro_library::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/the_macro_library)

include(CMakePackageConfigHelpers)
configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/the_macro_libraryConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/the_macro_library
)
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/the_macro_libraryConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/the_macro_libraryConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/the_macro_libraryConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/the_macro_library
)
# Extra project-specific targets
# Custom command for indent
add_custom_target(
    indent
    COMMAND python3 ${CMAKE_SOURCE_DIR}/bin/indent-macros
                    ${CMAKE_SOURCE_DIR}/include/*/*.h
                    ${CMAKE_SOURCE_DIR}/include/*/*/*.h
    COMMENT "Indenting files..."
)

# Custom target for building examples
add_custom_target(examples
    COMMAND ${CMAKE_COMMAND} -E echo "Building examples"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/examples"
    COMMAND ${CMAKE_COMMAND} -E chdir "${CMAKE_BINARY_DIR}/examples"
                             ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=$<CONFIG> "${CMAKE_SOURCE_DIR}/examples"
    COMMAND ${CMAKE_COMMAND} --build "${CMAKE_BINARY_DIR}/examples"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Building examples"
)

install(PROGRAMS ${CMAKE_SOURCE_DIR}/bin/convert-macros-to-code
                 ${CMAKE_SOURCE_DIR}/bin/indent-macros
        DESTINATION ${CMAKE_INSTALL_BINDIR})


enable_testing()
add_subdirectory(tests)
