cmake_minimum_required(VERSION 3.10)
project(sfcpp)

add_subdirectory(test)

set(CMAKE_CXX_STANDARD 17)

set(COMMON_SRC
    sf.cpp)

add_library(${PROJECT_NAME}
  ${COMMON_SRC})

