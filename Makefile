CC=g++
CFLAGS=-Wall -Wextra -std=c++20 `pkg-config --cflags sfml-all` -ggdb
LIBS=`pkg-config --libs sfml-all`
SRC=src/Config.cpp src/Eye.cpp src/main.cpp src/NeuralNetwork.cpp src/Path.cpp src/Ray.cpp src/Spline.cpp src/Car.cpp
default:
	$(CC) -o Car $(SRC) $(CFLAGS) $(LIBS)	
