#include "neural_network.hpp"

// Constants
#define SCALING_FACTOR_WEIGHTS 20


// https://www.geeksforgeeks.org/machine-learning/ml-neural-network-implementation-in-c-from-scratch/


// Activation functions
Scalar activationFunction(Scalar x, bool use_relu)
{
    if (use_relu){
        // ReLU
        if (x > 0)
            return x;
        else
            return 0;
    }
    else{
        // Tanh
        return tanhf(x);
    }

    
}

NeuralNetwork::NeuralNetwork(std::vector<uint> topology, bool random_init)
{
    this->topology = topology;

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
                    //(*weights.back()) *= SCALING_FACTOR_WEIGHTS;
                    // Create a random device and seed the generator
                    std::random_device rd;
                    std::mt19937 gen(rd());

                    // Define distribution between 0.0 and NUM_NEURONS
                    std::uniform_int_distribution<int> distr_int(0, topology[i]);

                    weights.back()->coeffRef(distr_int(gen)) *= SCALING_FACTOR_WEIGHTS;
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
                    //(*weights.back()) *= SCALING_FACTOR_WEIGHTS;
                    // Create a random device and seed the generator
                    std::random_device rd;
                    std::mt19937 gen(rd());

                    // Define distribution between 0.0 and NUM_NEURONS
                    std::uniform_int_distribution<int> distr_int(0, topology[i]);

                    weights.back()->coeffRef(distr_int(gen)) *= SCALING_FACTOR_WEIGHTS;
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

// Copy constructor - deep copy
NeuralNetwork::NeuralNetwork(const NeuralNetwork& other)
    : topology(other.topology)
{
    for (const auto* layer : other.neuronLayers) {
        neuronLayers.push_back(new RowVector(*layer));
    }
    for (const auto* weight : other.weights) {
        weights.push_back(new Matrix(*weight));
    }
}

// Copy assignment operator
NeuralNetwork& NeuralNetwork::operator=(const NeuralNetwork& other)
{
    if (this != &other) {
        // Clean up existing resources
        for (auto* layer : neuronLayers) delete layer;
        for (auto* weight : weights) delete weight;
        neuronLayers.clear();
        weights.clear();

        // Copy data
        topology = other.topology;

        for (const auto* layer : other.neuronLayers) {
            neuronLayers.push_back(new RowVector(*layer));
        }
        for (const auto* weight : other.weights) {
            weights.push_back(new Matrix(*weight));
        }
    }
    return *this;
}

// Move constructor
NeuralNetwork::NeuralNetwork(NeuralNetwork&& other) noexcept
    : neuronLayers(std::move(other.neuronLayers)),
      weights(std::move(other.weights)),
      topology(std::move(other.topology))
{
}

// Move assignment operator
NeuralNetwork& NeuralNetwork::operator=(NeuralNetwork&& other) noexcept
{
    if (this != &other) {
        // Clean up existing resources
        for (auto* layer : neuronLayers) delete layer;
        for (auto* weight : weights) delete weight;

        // Move data
        neuronLayers = std::move(other.neuronLayers);
        weights = std::move(other.weights);
        topology = std::move(other.topology);
    }
    return *this;
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
            neuronLayers[i]->coeffRef(j) = activationFunction(neuronLayers[i]->coeffRef(j), false);
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
    std::ofstream file(filepath, std::ios_base::app | std::ios_base::out);
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