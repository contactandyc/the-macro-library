cmake_minimum_required(VERSION 3.10)

project(MyProject)

# Custom command to run the Python script to convert the .cc file
add_custom_command(
  OUTPUT speed_test_d.cc
  COMMAND ${CMAKE_SOURCE_DIR}/bin/convert-macros-to-code ${CMAKE_SOURCE_DIR}/examples/speed-test/speed_test.cc > speed_test_d.cc
  DEPENDS ${CMAKE_SOURCE_DIR}/examples/speed-test/speed_test.cc
)

# Create an executable using the generated source
add_executable(speed_test speed_test.cc)
add_executable(speed_test_d speed_test_d.cc)

# Set the executable to be built with debug information
target_compile_options(speed_test_d PRIVATE -g)
