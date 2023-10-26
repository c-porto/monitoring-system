cmake_minimum_required(VERSION 3.15)
add_subdirectory(${CMAKE_SOURCE_DIR}/lib)
add_subdirectory(${CMAKE_SOURCE_DIR}/test)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(tests)
