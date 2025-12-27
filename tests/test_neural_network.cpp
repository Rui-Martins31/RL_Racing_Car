#include "../utils/neural_network.hpp"


// Globals
#define DEBUG true

#define TOPOLOGY {3, 9, 3}
#define PATH_OUTPUT "tests/test_csv.csv"

// Initialize tests

int test_01(int result, bool debug);
int test_02(bool debug);

int main(){
    test_01(70, DEBUG);
    test_02(DEBUG);

    return 0;
}

// Initialize NeuralNetwork and get weights
int test_01(int result, bool debug) {
    if (debug)
        std::cout << " -- Test 1 -- " << std::endl;


    NeuralNetwork test_nn(
        TOPOLOGY,
        true,
        0.005
    );

    // Weights
    int weights_num_total = test_nn.getWeightCount();
    auto weights          = test_nn.getWeights();

    if (debug) {
        std::cout << "NN Weight Count: " << weights_num_total << std::endl;
        std::cout << "NN Weights: "      << std::endl;
        for (size_t i = 0; i < weights_num_total; i++)
        {
            std::cout << "  " << std::to_string(weights[i]) << std::endl;
        }
    }

    if (weights_num_total == result)
        return 1;
    else
        return -1;
}

// Initiliaze NeuralNetwork with random weights and save them to csv
int test_02(bool debug) {
    if (debug)
        std::cout << " -- Test 2 -- " << std::endl;


    NeuralNetwork test_nn(
        TOPOLOGY,
        true,
        0.005
    );

    // Test variables
    int test_episode = 0;
    int test_reward  = 10;
    
    // Store values
    test_nn.saveToCSV(
        PATH_OUTPUT,
        test_episode,
        test_reward
    );

    if (debug)
        std::cout << "Wrote to file " << PATH_OUTPUT << std::endl;

    return 1;
}