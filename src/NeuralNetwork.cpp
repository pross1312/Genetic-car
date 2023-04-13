#include "NeuralNetwork.h"
#include <algorithm>
#include <iostream>
using namespace Eigen;

NeuralNetwork::NeuralNetwork(const NeuralNetwork& base) {
    this->topology = base.topology;
    this->activation = base.activation;
    for (int i = 0; i < base.weights.size(); i++) {
        // init random weights between -1 and 1
        this->weights.push_back(std::make_shared<MatrixXf>(*base.weights[i]));
        // change weights to between 0 and 1
        // *weights[weights.size()-1] = 0.5 * (*weights[weights.size()-1] + MatrixXf::Constant(layerSizes[i], layerSizes[i-1], 1));
        // init biases with 0
        this->biases.push_back(std::make_shared<VectorXf>(*base.biases[i]));
    }
}

NeuralNetwork::NeuralNetwork(std::vector<int> layerSizes,
                             std::function<float(float)> activation)
        : topology{layerSizes}, activation{activation} {
    if (layerSizes.size() < 2)
        throw std::invalid_argument("Network number of layers can't be less than 2.");
    if (*std::min_element(layerSizes.begin(), layerSizes.end()) < 1)
        throw std::invalid_argument("Layer size can't be less than 1.");

    for (int i = 1; i < layerSizes.size(); i++) {
        // init random weights between -1 and 1
        weights.push_back(std::make_shared<MatrixXf>(MatrixXf::Random(layerSizes[i], layerSizes[i-1])));
        // change weights to between 0 and 1
        *weights[weights.size()-1] = 0.5 * (*weights[weights.size()-1] + MatrixXf::Constant(layerSizes[i], layerSizes[i-1], 1));
        // init biases with 0
        biases.push_back(std::make_shared<VectorXf>(VectorXf::Random(layerSizes[i])));
        
    }
}

void NeuralNetwork::changeRandom() {
    for (int i = 0; i < weights.size(); i++) {
        *weights[i] += 2 * MatrixXf::Random(weights[i]->rows(), weights[i]->cols());
        *biases[i] += 2 * VectorXf::Random(biases[i]->size());
    }
}

NeuralNetwork NeuralNetwork::reproduce(const NeuralNetwork& n) const {
    NeuralNetwork temp{*this};
    for (int i = 0; i < temp.weights.size(); i++) {
        *temp.weights[i] = (*temp.weights[i] + *n.weights[i]) * 0.5f;
        *temp.biases[i] = (*temp.biases[i] + *n.biases[i]) * 0.5f;
    }
    return temp;
}

std::ofstream& operator<<(std::ofstream& fout, const NeuralNetwork& nn) {
    fout << nn.topology.size() << std::endl;
    for (const int& i : nn.topology)
        fout << i << " ";
    fout << std::endl;
    for (const auto& weight : nn.weights)
        fout << std::scientific << std::setprecision(10) << *weight << std::endl;
    for (const auto& bias : nn.biases)
        fout << std::scientific << std::setprecision(10) << *bias << std::endl;
    return fout;
}

std::ifstream& operator>>(std::ifstream& fin, NeuralNetwork& nn) {
    int n = 0;
    fin >> n;
    if (n != nn.topology.size())
        throw std::runtime_error("Wrong topology size!");
    std::vector<int> tempTopology(n);
    for (int& i : tempTopology)
        fin >> i;
    if (tempTopology != nn.topology)
        throw std::runtime_error("Wrong topology!");
    for (auto& weight : nn.weights) {
        for (int i = 0; i < weight->rows(); i++)
            for (int j = 0; j < weight->cols(); j++)
                fin >> weight->coeffRef(i, j);
    }
    for (auto& bias : nn.biases) {
        for (int i = 0; i < bias->size(); i++)
            fin >> bias->coeffRef(i);
    }
    return fin;
}


VectorXf NeuralNetwork::forward_propagate(VectorXf input) const {
    if (input.size() != topology[0])
        throw std::invalid_argument("Invalid input -- input must have same size as input layer in network.");
    for (int i = 0; i < weights.size(); i++) {
        input = ((*weights[i]) * input + (*biases[i])).unaryExpr(activation);
    }
    
    return input;
}

