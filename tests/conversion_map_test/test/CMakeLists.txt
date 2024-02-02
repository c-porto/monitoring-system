cmake_minimum_required(VERSION 3.15)
project(test)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)

enable_testing()

add_executable(${PROJECT_NAME} test.cc)

add_compile_options(
  -Wall -Wpedantic
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/lib/)

target_link_libraries(${PROJECT_NAME}
 PRIVATE
  GTest::GTest
  )

target_link_libraries(${PROJECT_NAME} PUBLIC -L${CMAKE_SOURCE_DIR}/build/lib/libmap.a)

