cmake_minimum_required(VERSION 3.10)
project(themacrolibrary VERSION 0.0.3)

option(DEBUG "Enable debugging" OFF)
option(ADDRESS_SANITIZER "Enable Address Sanitizer" OFF)

set(CMAKE_INSTALL_INCLUDEDIR include)
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
    COMMAND python3 ${CMAKE_SOURCE_DIR}/bin/indent-macros.py ${CMAKE_SOURCE_DIR}/include/*.h ${CMAKE_SOURCE_DIR}/include/*/*.h
    COMMENT "Indenting files..."
)

# Install README, AUTHORS, etc.
install(FILES README.md AUTHORS NEWS.md CHANGELOG.md LICENSE DESTINATION ${CMAKE_INSTALL_DOCDIR})

# Install header files
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

add_subdirectory(examples)
