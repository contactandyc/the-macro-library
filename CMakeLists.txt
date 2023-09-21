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

add_custom_target(
    indent
    COMMAND python3 ${CMAKE_SOURCE_DIR}/bin/indent-macros ${CMAKE_SOURCE_DIR}/include/*.h ${CMAKE_SOURCE_DIR}/include/*/*.h
    COMMENT "Indenting files..."
)

# install convert-macros-to-code, indent-macros
install(FILES ${CMAKE_SOURCE_DIR}/bin/convert-macros-to-code ${CMAKE_SOURCE_DIR}/bin/indent-macros DESTINATION ${CMAKE_INSTALL_BINDIR}
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

# Install README, AUTHORS, etc.
install(FILES README.md AUTHORS NEWS.md CHANGELOG.md LICENSE NOTICE DESTINATION ${CMAKE_INSTALL_DOCDIR})

# Install header files
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/the-macro-library DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/the-macro-library)

install(CODE "execute_process(COMMAND git log -n1 OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/GIT_REF)")
install(FILES LICENSE NOTICE ${CMAKE_CURRENT_BINARY_DIR}/GIT_REF DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/the-macro-library)

add_subdirectory(examples)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/bin/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)