cmake_minimum_required(VERSION 3.15)

set(this linuxhost)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_EXPORT_COMPILE_COMMANDS On)
set(server server.cc)

project(${this} CXX)

file(GLOB SRC_FILES libs/*.cc)

add_subdirectory(third_party/abseil-cpp)

add_executable(${this} ${server} ${SRC_FILES})

target_include_directories(${this} INTERFACE ${CMAKE_SOURCE_DIR}/libs)

target_link_libraries(${this} absl::strings)




