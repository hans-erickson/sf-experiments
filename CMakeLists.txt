cmake_minimum_required(VERSION 3.10)
project(sfexp)

add_subdirectory(c++-wrapper)

set(CMAKE_CXX_STANDARD 17)


add_executable(speed-cycle speed-cycle.cpp)

target_link_libraries(speed-cycle sfcpp sndfile)


add_executable(accel-decel accel-decel.cpp)

target_link_libraries(accel-decel sfcpp sndfile)
