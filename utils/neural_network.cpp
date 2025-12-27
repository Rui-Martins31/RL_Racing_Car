#include "neural_network.hpp"
#include <sstream>
#include <cmath>

// https://www.geeksforgeeks.org/machine-learning/ml-neural-network-implementation-in-c-from-scratch/


// Activation functions
Scalar activationFunction(Scalar x)
{
    return tanhf(x);
}

NeuralNetwork::NeuralNetwork(std::vector<uint> topology, bool random_init, Scalar learningRate)
{
    this->topology = topology;
    this->learningRate = learningRate;

    for (uint i = 0; i < topology.size(); i++) {
        // Initialize neuron layers
        if (i == topology.size() - 1)
            neuronLayers.push_back(new RowVector(topology[i]));
        else
            neuronLayers.push_back(new RowVector(topology[i] + 1));

        // Set bias neurons to 1.0
        if (i != topology.size() - 1) {
            neuronLayers.back()->coeffRef(topology[i]) = 1.0;
        }

        // Initialize weights matrix
        if (i > 0) {
            if (i != topology.size() - 1) {
                weights.push_back(new Matrix(topology[i - 1] + 1, topology[i] + 1));
                if (random_init) {
                    weights.back()->setRandom();
                } else {
                    weights.back()->setZero();
                }
                weights.back()->col(topology[i]).setZero();
                weights.back()->coeffRef(topology[i - 1], topology[i]) = 1.0;
            }
            else {
                weights.push_back(new Matrix(topology[i - 1] + 1, topology[i]));
                if (random_init) {
                    weights.back()->setRandom();
                } else {
                    weights.back()->setZero();
                }
            }
        }
    }
}

NeuralNetwork::~NeuralNetwork()
{
    for (auto* layer : neuronLayers) delete layer;
    for (auto* weight : weights)     delete weight;
}

RowVector NeuralNetwork::propagateForward(RowVector& input)
{
    // Set the input to input layer (excluding bias neuron)
    neuronLayers.front()->block(0, 0, 1, neuronLayers.front()->size() - 1) = input;

    // Propagate through each layer
    for (uint i = 1; i < topology.size(); i++) {
        // Matrix multiplication
        (*neuronLayers[i]) = (*neuronLayers[i - 1]) * (*weights[i - 1]);

        // Apply activation function to non-bias neurons
        for (uint j = 0; j < topology[i]; j++) {
            neuronLayers[i]->coeffRef(j) = activationFunction(neuronLayers[i]->coeffRef(j));
        }
    }

    // Return the output layer
    return *neuronLayers.back();
}

std::vector<Scalar> NeuralNetwork::getWeights() const
{
    std::vector<Scalar> flatWeights;

    for (const auto* weightMatrix : weights) {
        for (int row = 0; row < weightMatrix->rows(); row++) {
            for (int col = 0; col < weightMatrix->cols(); col++) {
                flatWeights.push_back((*weightMatrix)(row, col));
            }
        }
    }

    return flatWeights;
}

void NeuralNetwork::setWeights(const std::vector<Scalar>& flatWeights)
{
    size_t idx = 0;

    for (auto* weightMatrix : weights) {
        for (int row = 0; row < weightMatrix->rows(); row++) {
            for (int col = 0; col < weightMatrix->cols(); col++) {
                if (idx < flatWeights.size()) {
                    (*weightMatrix)(row, col) = flatWeights[idx++];
                }
            }
        }
    }
}

size_t NeuralNetwork::getWeightCount() const
{
    size_t count = 0;
    for (const auto* weightMatrix : weights) {
        count += weightMatrix->rows() * weightMatrix->cols();
    }
    return count;
}

void NeuralNetwork::saveToCSV(const std::string& filepath, int episode, float reward) const
{
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << filepath << std::endl;
        return;
    }

    // [episode, reward, w1, w2, ...]
    file << episode << "," << reward;

    std::vector<Scalar> flatWeights = getWeights();
    for (const auto& w : flatWeights) {
        file << "," << w;
    }
    file << "\n";

    file.close();
}

NeuralNetwork NeuralNetwork::loadFromCSV(const std::string& filepath, std::vector<uint> topology)
{
    NeuralNetwork nn(topology, false);

    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for reading: " << filepath << std::endl;
        return nn;
    }

    std::string line;
    if (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token;

        // Skip episode number
        std::getline(ss, token, ',');
        // Skip reward
        std::getline(ss, token, ',');

        // Read weights
        std::vector<Scalar> flatWeights;
        while (std::getline(ss, token, ',')) {
            flatWeights.push_back(std::stof(token));
        }

        nn.setWeights(flatWeights);
    }

    file.close();
    return nn;
}