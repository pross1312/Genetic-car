cmake_minimum_required(VERSION 3.13)
project(genetic-car)

set(CMAKE_FLAGS "${CMAKE_FLAGS} -Wall -Wextra -ggdb -O3")

include_directories(src/)

add_executable(Car
    src/NeuralNetwork.h
    src/Path.h
    src/Eye.h
    src/Spline.h
    src/Helper.h
    src/Eye.cpp
    src/main.cpp
    src/Config.h
    src/Car.cpp
    src/Path.cpp
    src/lin_al.h
    src/NeuralNetwork.cpp
    src/Spline.cpp
    src/Config.cpp
    src/Ray.cpp
    src/Car.h
    src/Ray.h)

target_link_libraries(Car sfml-graphics)
target_link_libraries(Car sfml-window)
target_link_libraries(Car sfml-system)
