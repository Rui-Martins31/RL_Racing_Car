#ifndef NEURAL_NETWORK_HPP
#define NEURAL_NETWORK_HPP

// Imports
#include <eigen3/Eigen/Eigen>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>

// Types
typedef float Scalar;
typedef Eigen::MatrixXf Matrix;
typedef Eigen::RowVectorXf RowVector;
typedef Eigen::VectorXf ColVector;

// Activation functions
Scalar activationFunction(Scalar x, bool use_relu);

class NeuralNetwork {
public:
    // Constructor
    // random_init: true  = random weights
    //              false = zero weights
    NeuralNetwork(std::vector<uint> topology, bool random_init = true);

    // Copy constructor and assignment
    NeuralNetwork(const NeuralNetwork& other);
    NeuralNetwork& operator=(const NeuralNetwork& other);

    // Move constructor and assignment
    NeuralNetwork(NeuralNetwork&& other) noexcept;
    NeuralNetwork& operator=(NeuralNetwork&& other) noexcept;

    // Destructor
    ~NeuralNetwork();

    // Forward pass
    RowVector propagateForward(RowVector& input);

    // Weight manipulation for evolutionary algorithm
    std::vector<Scalar> getWeights() const;
    void setWeights(const std::vector<Scalar>& weights);
    size_t getWeightCount() const;

    // CSV serialization
    void saveToCSV(const std::string& filepath, int episode, float reward) const;
    static NeuralNetwork loadFromCSV(const std::string& filepath, std::vector<uint> topology);

    // Storage objects for neural network
    std::vector<RowVector*> neuronLayers; // stores the different layers of our network
    std::vector<Matrix*> weights;         // the connection weights

    std::vector<uint> topology;
};

#endif