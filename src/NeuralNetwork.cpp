#include "NeuralNetwork.h"
#include <algorithm>
#include <iostream>

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

NeuralNetwork::NeuralNetwork(std::vector<size_t> layerSizes,
                             std::function<float(float)> activation)
        : topology{layerSizes}, activation{activation} {
    if (layerSizes.size() < 2)
        throw std::invalid_argument("Network number of layers can't be less than 2.");
    if (*std::min_element(layerSizes.begin(), layerSizes.end()) < 1)
        throw std::invalid_argument("Layer size can't be less than 1.");

    for (size_t i = 1; i < layerSizes.size(); i++) {
        weights.push_back(MatrixXf(layerSizes[i], layerSizes[i-1], randf));
        biases.push_back(VectorXf(layerSizes[i], randf));
    }
}

void NeuralNetwork::changeRandom() {
    for (size_t i = 0; i < weights.size(); i++) {
        weights[i] += MatrixXf(weights[i].nRows, weights[i].nCols, randf)*.5f;
        biases[i] += VectorXf(biases[i].size(), randf)*0.5f;
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
    size_t n = nn.topology.size();
    fout.write((char*)&n, sizeof(nn.topology.size()));
    fout.write((char*)nn.topology.data(), nn.topology.size()*sizeof(nn.topology[0]));
    for (size_t i = 0; i < nn.weights.size(); i++) {
        fout.write((char*)nn.weights[i].data, nn.weights[i].size()*sizeof(float));
        fout.write((char*)nn.biases[i].data, nn.biases[i].size()*sizeof(float));
    }
    return fout;
}

std::ifstream& operator>>(std::ifstream& fin, NeuralNetwork& nn) {
    size_t n = 0;
    fin.read((char*)&n, sizeof(n));
    assert(n >= 2 && "at least 2 layers(input, output)");
    if (nn.topology.size() != n) nn.topology.resize(n);
    fin.read((char*)nn.topology.data(), nn.topology.size() * sizeof(nn.topology[0]));
    nn.weights.clear();
    nn.biases.clear();
    for (size_t i = 1; i < n; i++) {
        MatrixXf temp_w(nn.topology[i], nn.topology[i-1]);
        fin.read((char*)temp_w.data, temp_w.size()*sizeof(float));
        nn.weights.push_back(temp_w);
        VectorXf temp_b(nn.topology[i]);
        fin.read((char*)temp_b.data, temp_b.size()*sizeof(float));
        nn.biases.push_back(temp_b);
    }
    return fin;
}

VectorXf NeuralNetwork::forward_propagate(VectorXf input) const {
    if (input.size() != topology[0])
        throw std::invalid_argument("Invalid input -- input must have same size as input layer in network.");
    for (size_t i = 0; i < weights.size(); i++) {
        const auto& temp = (weights[i] * input + biases[i]).unary_transformed(activation);
        input.replace_with(temp);
    }

    return input;
}
