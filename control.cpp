#include <iostream>
#include <cmath>

// Custom scripts
#include "control.hpp"

// Globals
#define DEBUG false
#define PATH_OUTPUT "output.csv"
#define EPISODE_MAX 1000

#define NN_CONTROL true
#define NN_TOPOLOGY {3, 3, 3}
#define NN_LEARNING_RATE 0.005

/*
Rewards:
    - Out of bounds: -10
    - Complete lap: +10
    - Time on track: +1 * #min
    - Fastest lap: +5
*/

#define MAX_SPEED 40.0

float velocity(float vel_x, float vel_y, float vel_z)
{
    return sqrt( pow(vel_x, 2) + pow(vel_y, 2) + pow(vel_z, 2) );
}

int reward(/* args */)
{
    return 10;
}

MessageClient control(int episode_num, int episode_cycles, MessageServer message)
{   
    // Control struct
    MessageClient control;

    // Neural Network
    NeuralNetwork nn(
        NN_TOPOLOGY,
        true,
        NN_LEARNING_RATE
    );

    if (NN_CONTROL) {
        // Control based on NeuralNetwork
        // Inputs {vel, ang, pos}
        RowVector inputs(3);
        inputs[0] = velocity(message.speedX, message.speedY, message.speedZ), 0.0, 1.0;
        inputs[1] = message.angle;
        inputs[2] = message.trackPos;

        // Forward {accel, brake, steer}
        RowVector outputs = nn.propagateForward(inputs);

        // Output
        control.accel = outputs[0];
        control.brake = outputs[1];
        control.steer = outputs[2];
    }
    else {
        // Control based on distance to the central point
        // Gas control
        float vel = velocity(message.speedX, message.speedY, message.speedZ);
        
        if (vel <= MAX_SPEED) { control.accel = 0.25; }
        else { control.accel = 0.0; }
        
        // Brake control
        control.brake = 0.0;
        
        // Steering control
        float curr_angle = message.angle;    // Angle: [-Pi, Pi]
        float curr_pos   = message.trackPos; // TrackPos: [-1, 1]
        control.steer    = -curr_pos; // - (curr_angle/3.1416);
    }
    
    // DO NOT CHANGE ----
    control.clutch = 0.0;
    control.focus  = 0.0;
    control.gear   = 1;
    
    // Check if car is out of track
    if ((message.trackPos < -1.0 || message.trackPos > 1.0) || (episode_cycles % EPISODE_MAX == 0)) {
        control.meta = true;

        if (NN_CONTROL) {
            // Store weights
            int final_reward = reward();
            nn.saveToCSV(
                PATH_OUTPUT,
                episode_num,
                final_reward
            );
        }
    }
    else control.meta = false;
    // DO NOT CHANGE ----

    // DEBUG 
    std::cout << "Episode cycles: " << episode_cycles << std::endl;
    
    return control;
}