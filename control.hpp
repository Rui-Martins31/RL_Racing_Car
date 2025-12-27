#ifndef CONTROL_HPP
#define CONTROL_HPP

#include "utils/parser.hpp"
#include "utils/neural_network.hpp"

// Auxiliary functions
float velocity(float vel_x, float vel_y, float vel_z);
int reward(/* args */);

// Control based on the current sensor values
MessageClient control(int episode_num, int episode_cycles, MessageServer message);

#endif