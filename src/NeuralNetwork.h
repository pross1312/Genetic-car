#pragma once
#include <iomanip>
#include <vector>
#include <cmath>
#include <functional>
#include <eigen3/Eigen/Core>
#include <fstream>
#include <memory>
#include <exception>

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
	NeuralNetwork(std::vector<int> layerSizes, std::function<float(float)> activation);
    NeuralNetwork(const NeuralNetwork& a, const NeuralNetwork& b);
	Eigen::VectorXf forward_propagate(Eigen::VectorXf input) const;
    NeuralNetwork& operator=(const NeuralNetwork& b);
	friend std::ifstream& operator>>(std::ifstream& fin, NeuralNetwork& nn);
	friend std::ofstream& operator<<(std::ofstream& fout, const NeuralNetwork& nn);
	void changeRandom();
    NeuralNetwork reproduce(const NeuralNetwork& n) const;
private:
	std::vector<int> topology;
	std::vector<std::shared_ptr<Eigen::MatrixXf>> weights;
	std::vector<std::shared_ptr<Eigen::VectorXf>> biases;
	std::function<float(float)> activation;
};
