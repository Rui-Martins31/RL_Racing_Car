#ifndef CONTROL_HPP
#define CONTROL_HPP

#include <iostream>
#include <cmath>
#include <fstream>
#include <sstream>
#include <map>
#include <algorithm>
#include <bits/stdc++.h>

#include "utils/parser.hpp"
#include "utils/neural_network.hpp"

// Auxiliary functions
float velocity(float vel_x, float vel_y, float vel_z);
int reward(bool out_of_bounds, float dist_raced, float last_lap_time);
float remap(float value, float original_min, float original_max,
                         float new_min, float new_max);

// Agent
class Agent
{
public:
    // Variables
    NeuralNetwork nn;
    int id;
    float reward;
    MessageClient previous_message;

    // Methods
    Agent(int agent_num);
    Agent(int agent_num, float agent_reward, const std::vector<Scalar>& weights);
    ~Agent() {};
};


// Generation
class Generation
{
private:
    // Constants
    const int AGENTS_NUM_TOTAL   = 50; // 50
    const int AGENTS_NUM_SURVIVE = 25; // 25

    const float AGENT_PROB_NEW   = 0.10; // Probability of a new agent being born

    const float MUTATION_PROB    = 0.75; // Probability of occuring a mutation
    const float MUTATION_CHANGE  = 0.05;

    // Variables
    std::vector<Agent> arr_agents;
    int generation_num_curr;
    int agent_num_curr;
    
public:
    // Constructor and Destructor
    Generation();
    ~Generation() {};

    // Methods
    bool load_last_complete_generation(const std::string& filepath);
    MessageClient step(int episode_cycles, MessageServer message);
    void update(float reward);
    void populate();

    // Getters and Setters
    int get_current_generation_num() 
        {return this->generation_num_curr;};
    int get_current_agent_num()
        {return this->agent_num_curr;};
    std::vector<Scalar> get_weights_from_agent(int agent_num)
        {return this->arr_agents[agent_num].nn.getWeights();};
};

#endif