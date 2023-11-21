cmake_minimum_required(VERSION 3.15)

set(this linuxhost)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_EXPORT_COMPILE_COMMANDS On)
set(server server.cc)

file(GLOB SRC_FILES libs/*.cc)

project(${this} CXX)

add_executable(${this} ${server} ${SRC_FILES})
add_compile_options(-Wall -Wpedantic -O3)

target_include_directories(${this} INTERFACE ${CMAKE_SOURCE_DIR}/libs)


