cmake_minimum_required(VERSION 3.10)
project(MyProject)

# Get a list of all .c and .cc files in the current directory
file(GLOB C_FILES *.c)
file(GLOB CC_FILES *.cc)

# Loop through each .c file to create its target
foreach(file ${C_FILES})
    get_filename_component(name ${file} NAME_WE)  # Remove directory and extension
    add_custom_command(
        OUTPUT ${name}_d.c
        COMMAND ${CMAKE_SOURCE_DIR}/bin/convert-macros-to-code ${CMAKE_SOURCE_DIR}/examples/demo/${name}.c > ${name}_d.c
        DEPENDS ${CMAKE_SOURCE_DIR}/examples/demo/${name}.c
    )
    add_executable(${name} ${name}.c)
    add_executable(${name}_d ${name}_d.c)
    target_compile_options(${name}_d PRIVATE -I${CMAKE_SOURCE_DIR}/include -g)
endforeach()

# Loop through each .cc file to create its target
foreach(file ${CC_FILES})
    get_filename_component(name ${file} NAME_WE)  # Remove directory and extension
    add_custom_command(
        OUTPUT ${name}_d.cc
        COMMAND ${CMAKE_SOURCE_DIR}/bin/convert-macros-to-code ${CMAKE_SOURCE_DIR}/examples/demo/${name}.cc > ${name}_d.cc
        DEPENDS ${CMAKE_SOURCE_DIR}/examples/demo/${name}.cc
    )
    add_executable(${name} ${name}.cc)
    add_executable(${name}_d ${name}_d.cc)
    target_compile_options(${name}_d PRIVATE -I${CMAKE_SOURCE_DIR}/include -g)
endforeach()
