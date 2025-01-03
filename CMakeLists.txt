cmake_minimum_required(VERSION 3.10)

# Project Configuration
project(themacrolibrary VERSION 0.1.1)

# Variables
set(INCLUDE_DIR_NAME "the-macro-library")
set(EXTRA_FILES README.md AUTHORS NEWS.md CHANGELOG.md LICENSE NOTICE)

find_package(a-cmake-library REQUIRED)

include(LibraryConfig)
include(LibraryBuild)

# Custom command for indent
add_custom_target(
    indent
    COMMAND python3 ${CMAKE_SOURCE_DIR}/bin/indent-macros ${CMAKE_SOURCE_DIR}/include/*/*.h ${CMAKE_SOURCE_DIR}/include/*/*/*.h
    COMMENT "Indenting files..."
)

# Examples directory
# add_subdirectory(examples)
# Custom target for building examples
add_custom_target(examples
    COMMAND ${CMAKE_COMMAND} -E echo "Building examples"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/examples"
    COMMAND ${CMAKE_COMMAND} -E chdir "${CMAKE_BINARY_DIR}/examples" ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=$<CONFIG> "${CMAKE_SOURCE_DIR}/examples"
    COMMAND ${CMAKE_COMMAND} --build "${CMAKE_BINARY_DIR}/examples"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Building examples"
)

if(NOT CMAKE_INSTALL_BINDIR)
    set(CMAKE_INSTALL_BINDIR bin)
endif()

# Install scripts
install(PROGRAMS ${CMAKE_SOURCE_DIR}/bin/convert-macros-to-code ${CMAKE_SOURCE_DIR}/bin/indent-macros
        DESTINATION ${CMAKE_INSTALL_BINDIR})

