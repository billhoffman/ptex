cmake_minimum_required(VERSION 2.8)
project(ptex)

# enable testing for the project
include(CTest)
enable_testing()

# add global include directory
include_directories(${ptex_SOURCE_DIR}/ptex)

# add sub directories
add_subdirectory(ptex)
add_subdirectory(utils)
add_subdirectory(tests)

