
cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TEST_NAME UTs)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

add_executable(${TEST_NAME} threadpool_tests.cpp)
target_link_libraries(${TEST_NAME} ${PROJECT_NAME}-lib)
target_link_libraries(${TEST_NAME} GTest::gtest_main)

gtest_discover_tests(${TEST_NAME})

add_custom_command(TARGET ${TEST_NAME} POST_BUILD COMMAND ${TEST_NAME})