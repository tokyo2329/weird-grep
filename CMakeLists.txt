cmake_minimum_required(VERSION 3.20)

project(weird-grep)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(COMPILE_FLAGS
  -Wall -Wextra
)

set(SRC
  
)

add_library(${PROJECT_NAME}-lib STATIC ${SRC})

target_compile_options(
  ${PROJECT_NAME}-lib
  PRIVATE
  ${COMPILE_FLAGS}
)

target_include_directories(
  ${PROJECT_NAME}-lib
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}-lib)
