cmake_minimum_required(VERSION 3.10)
project(DemoExamples)

# Get a list of all .c and .cc files in the current directory
file(GLOB C_FILES *.c)
file(GLOB CC_FILES *.cc)

# Loop through each .cc file to create its target
foreach(file ${CC_FILES})
    get_filename_component(name ${file} NAME_WE)  # Remove directory and extension
    set(generated_file "${CMAKE_CURRENT_BINARY_DIR}/${name}_d.cc")

    add_custom_command(
        OUTPUT ${generated_file}
        COMMAND ${CMAKE_SOURCE_DIR}/../bin/convert-macros-to-code ${file} > ${generated_file}
        DEPENDS ${file}
        COMMENT "Generating ${generated_file} from ${file}"
    )

    add_executable(${name} ${file})
    add_executable(${name}_d ${generated_file})

    target_include_directories(${name}_d PRIVATE ${CMAKE_SOURCE_DIR}/../include)
    target_compile_options(${name}_d PRIVATE -g)
endforeach()

# Loop through each .c file to create its target
foreach(file ${C_FILES})
    get_filename_component(name ${file} NAME_WE)  # Remove directory and extension
    set(generated_file "${CMAKE_CURRENT_BINARY_DIR}/${name}_d.c")

    add_custom_command(
        OUTPUT ${generated_file}
        COMMAND ${CMAKE_SOURCE_DIR}/../bin/convert-macros-to-code ${file} > ${generated_file}
        DEPENDS ${file}
        COMMENT "Generating ${generated_file} from ${file}"
    )

    add_executable(${name}_d ${generated_file})
    add_executable(${name} ${file})

    target_include_directories(${name}_d PRIVATE ${CMAKE_SOURCE_DIR}/../include)
    target_compile_options(${name}_d PRIVATE -g)
endforeach()
