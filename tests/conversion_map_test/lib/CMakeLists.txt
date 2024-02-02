cmake_minimum_required(VERSION 3.16)

project(map CXX)

set(LIB_FILES ${CMAKE_SOURCE_DIR}/lib/map.h)
set(SRC_FILES ${CMAKE_SOURCE_DIR}/lib/map.cpp)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)

add_library(${PROJECT_NAME} STATIC ${SRC_FILES} ${LIB_FILES})
add_compile_options(
  -Wall -Wpedantic
)
target_include_directories(${PROJECT_NAME} PUBLIC .)
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
