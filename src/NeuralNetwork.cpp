#include "NeuralNetwork.h"
#include <algorithm>
#include <iostream>
using namespace Eigen;

NeuralNetwork::NeuralNetwork(const NeuralNetwork& base) {
    assert(this->weights.size() == 0);
    this->topology = base.topology;
    this->activation = base.activation;
    for (size_t i = 0; i < base.weights.size(); i++) {
        // init random weights between -1 and 1
        this->weights.push_back(base.weights[i]);
        this->biases.push_back(base.biases[i]);
    }
}
NeuralNetwork& NeuralNetwork::operator=(const NeuralNetwork& b) {
    this->topology = b.topology;
    this->activation = b.activation;
    this->weights.clear();
    this->biases.clear();
    for (size_t i = 0; i < b.weights.size(); i++) {
        this->weights.push_back(b.weights[i]);
        this->biases.push_back(b.biases[i]);
    }
    return *this;
}
NeuralNetwork::NeuralNetwork(const NeuralNetwork& a, const NeuralNetwork& b) {
    assert(a.topology == b.topology);
    this->topology = a.topology;
    this->activation = a.activation;
    for (size_t i = 0; i < a.weights.size(); i++) {
        float r = rand() * 1.0f / RAND_MAX;
        this->weights.push_back((a.weights[i] + b.weights[i]) * r);
        this->biases.push_back((a.biases[i] + b.biases[i]) * r);
    }
}

NeuralNetwork::NeuralNetwork(std::vector<int> layerSizes,
                             std::function<float(float)> activation)
        : topology{layerSizes}, activation{activation} {
    if (layerSizes.size() < 2)
        throw std::invalid_argument("Network number of layers can't be less than 2.");
    if (*std::min_element(layerSizes.begin(), layerSizes.end()) < 1)
        throw std::invalid_argument("Layer size can't be less than 1.");

    for (size_t i = 1; i < layerSizes.size(); i++) {
        weights.push_back(MatrixXf::Random(layerSizes[i], layerSizes[i-1]));
        weights[weights.size()-1] = 0.5 * (weights[weights.size()-1] + MatrixXf::Constant(layerSizes[i], layerSizes[i-1], 1));
        biases.push_back(VectorXf::Random(layerSizes[i]));
    }
}

void NeuralNetwork::changeRandom() {
    for (size_t i = 0; i < weights.size(); i++) {
        weights[i] += MatrixXf::Random(weights[i].rows(), weights[i].cols());
        biases[i] += VectorXf::Random(biases[i].size());
    }
}


NeuralNetwork NeuralNetwork::reproduce(const NeuralNetwork& n) const {
    NeuralNetwork child{*this};
    for (size_t i = 0; i < child.weights.size(); i++) {
        float ratio = rand()*1.0f / RAND_MAX;
        child.weights[i] = child.weights[i] * ratio + n.weights[i] * (1-ratio);
        child.biases[i]  = child.biases[i]  * ratio + n.biases[i]  * (1-ratio);
    }
    return child;
}

std::ofstream& operator<<(std::ofstream& fout, const NeuralNetwork& nn) {
    fout << nn.topology.size() << std::endl;
    for (const int& i : nn.topology)
        fout << i << " ";
    fout << std::endl;
    for (const auto& weight : nn.weights)
        fout << std::scientific << std::setprecision(10) << weight << std::endl;
    for (const auto& bias : nn.biases)
        fout << std::scientific << std::setprecision(10) << bias << std::endl;
    return fout;
}

std::ifstream& operator>>(std::ifstream& fin, NeuralNetwork& nn) {
    size_t n = 0;
    fin >> n;
    if (n != nn.topology.size())
        throw std::runtime_error("Wrong topology size!");
    std::vector<int> temp_topology(n);
    for (int& i : temp_topology)
        fin >> i;
    if (temp_topology != nn.topology)
        throw std::runtime_error("Wrong topology!");
    for (auto& weight : nn.weights) {
        for (int i = 0; i < weight.rows(); i++)
            for (int j = 0; j < weight.cols(); j++)
                fin >> weight.coeffRef(i, j);
    }
    for (auto& bias : nn.biases) {
        for (int i = 0; i < bias.size(); i++)
            fin >> bias.coeffRef(i);
    }
    return fin;
}


VectorXf NeuralNetwork::forward_propagate(VectorXf input) const {
    if (input.size() != topology[0])
        throw std::invalid_argument("Invalid input -- input must have same size as input layer in network.");
    for (size_t i = 0; i < weights.size(); i++) {
        input = ((weights[i]) * input + (biases[i])).unaryExpr(activation);
    }

    return input;
}
