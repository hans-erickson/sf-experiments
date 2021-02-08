cmake_minimum_required(VERSION 3.10)
project(wrappertest)


set(CMAKE_CXX_STANDARD 17)

set(COMMON_SRC
    main.cpp
    wrapper-test.cpp)

add_definitions(-DSF_TEST_SOUND_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\")

add_executable(${PROJECT_NAME}
  ${COMMON_SRC})

target_link_libraries(${PROJECT_NAME} gtest pthread sfcpp sndfile)
