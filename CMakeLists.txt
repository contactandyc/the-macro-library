cmake_minimum_required(VERSION 3.10)
project(themacrolibrary VERSION 0.1.1)

option(DEBUG "Enable debugging" OFF)
option(ADDRESS_SANITIZER "Enable Address Sanitizer" OFF)

set(CMAKE_INSTALL_INCLUDEDIR include)
set(CMAKE_INSTALL_BINDIR bin)
set(CMAKE_INSTALL_DOCDIR share/doc/themacrolibrary)
set(CMAKE_C_STANDARD 99)

if(DEBUG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I${CMAKE_SOURCE_DIR}/include -g -O0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I${CMAKE_SOURCE_DIR}/include -g -O0")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I${CMAKE_SOURCE_DIR}/include -O3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I${CMAKE_SOURCE_DIR}/include -O3")
endif()

if(ADDRESS_SANITIZER)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
endif()

# Custom command for indent
add_custom_target(
    indent
    COMMAND python3 ${CMAKE_SOURCE_DIR}/bin/indent-macros ${CMAKE_SOURCE_DIR}/include/*/*.h ${CMAKE_SOURCE_DIR}/include/*/*/*.h
    COMMENT "Indenting files..."
)

# Install scripts
install(FILES ${CMAKE_SOURCE_DIR}/bin/convert-macros-to-code ${CMAKE_SOURCE_DIR}/bin/indent-macros
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

# Install documentation
install(FILES README.md AUTHORS NEWS.md CHANGELOG.md LICENSE NOTICE
        DESTINATION ${CMAKE_INSTALL_DOCDIR})

# Create an interface library
add_library(themacrolibrary INTERFACE)

# Set include directories
target_include_directories(themacrolibrary INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

# Install header files
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/the-macro-library
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Install the library target
install(TARGETS themacrolibrary
        EXPORT ${PROJECT_NAME}Targets
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Export the targets
install(EXPORT ${PROJECT_NAME}Targets
        FILE ${PROJECT_NAME}Targets.cmake
        DESTINATION lib/cmake/${PROJECT_NAME})

# Create and install the config file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${PROJECT_NAME}ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

configure_file(${PROJECT_NAME}Config.cmake.in ${PROJECT_NAME}Config.cmake @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION lib/cmake/${PROJECT_NAME})

# Uninstall command
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/bin/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

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
