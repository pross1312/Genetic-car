cmake_minimum_required(VERSION 3.13)
project(genetic-car)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -ggdb -O2")

include_directories(src/)
file(GLOB SOURCES
    src/*.h
    src/*.cpp
)
add_executable(Car ${SOURCES})

target_link_libraries(Car sfml-graphics)
target_link_libraries(Car sfml-window)
target_link_libraries(Car sfml-system)
