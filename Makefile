CC=g++
CFLAGS=-Wall -Wextra -std=c++20 `pkg-config --cflags sfml-all eigen3`
LIBS=`pkg-config --libs sfml-all eigen3`
SRC=src/Car.cpp src/Config.cpp src/CubicCurve.cpp src/Eye.cpp src/main.cpp src/NeuralNetwork.cpp src/Path.cpp src/Ray.cpp src/Spline.cpp
default:
	$(CC) -o Car $(SRC) $(CFLAGS) $(LIBS)	
