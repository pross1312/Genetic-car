#pragma once
#include "lin_al.h"
#include <functional>
#include <iomanip>
#include <vector>
#include <cmath>
#include <fstream>
#include <memory>
#include <exception>

inline float randf() { return rand()*2.0f/RAND_MAX - 1.0f; } // random from -1 to 1

inline float tanh_activate(float x) {
    return (std::exp(x) - std::exp(-x)) / (std::exp(x) + std::exp(-x));
}

inline float sigmoid_activate(float x) {
	return 1 / (1 + std::exp(-x));
}

inline float relu_activate(float x) {
	return std::max(x, 0.0f);
}

class NeuralNetwork {
public:
	~NeuralNetwork() = default;
	NeuralNetwork(const NeuralNetwork& base);
	NeuralNetwork(std::vector<size_t> layerSizes, std::function<float(float)> activation);
    NeuralNetwork(const NeuralNetwork& a, const NeuralNetwork& b);
	VectorXf forward_propagate(VectorXf input) const;
    NeuralNetwork& operator=(const NeuralNetwork& b);
	friend std::ifstream& operator>>(std::ifstream& fin, NeuralNetwork& nn);
	friend std::ofstream& operator<<(std::ofstream& fout, const NeuralNetwork& nn);
	void changeRandom();
    NeuralNetwork reproduce(const NeuralNetwork& n) const;
private:
	std::vector<size_t> topology;
	std::vector<MatrixXf> weights;
	std::vector<VectorXf> biases;
	std::function<float(float)> activation;
};
