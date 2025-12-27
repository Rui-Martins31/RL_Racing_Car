#ifndef NEURAL_NETWORK_HPP
#define NEURAL_NETWORK_HPP

// Imports
#include <eigen3/Eigen/Eigen>
#include <iostream>
#include <vector>

// Types
typedef float Scalar;
typedef Eigen::MatrixXf Matrix;
typedef Eigen::RowVectorXf RowVector;
typedef Eigen::VectorXf ColVector;

class NeuralNetwork {
public:
    // constructor
    NeuralNetwork(std::vector<uint> topology, Scalar learningRate = Scalar(0.005));

    // forward pass
    void propagateForward(RowVector& input);

    // function to update the weights of connections
    void updateWeights();

    // storage objects for working of neural network
    /*
          use pointers when using std::vector<Class> as std::vector<Class> calls destructor of 
          Class as soon as it is pushed back! when we use pointers it can't do that, besides
          it also makes our neural network class less heavy!! It would be nice if you can use
          smart pointers instead of usual ones like this
        */
    std::vector<RowVector*> neuronLayers; // stores the different layers of out network
    std::vector<RowVector*> cacheLayers;  // stores the unactivated (activation fn not yet applied) values of layers
    std::vector<RowVector*> deltas;       // stores the error contribution of each neurons
    std::vector<Matrix*> weights;         // the connection weights itself

    Scalar learningRate;
    std::vector<uint> topology;
};

#endif