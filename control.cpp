#include <iostream>
#include <cmath>

// Custom scripts
#include "control.hpp"

// Globals
#define DEBUG false

#define MAX_SPEED 40.0

float velocity(float vel_x, float vel_y, float vel_z)
{
    return sqrt( pow(vel_x, 2) + pow(vel_y, 2) + pow(vel_z, 2) );
}

MessageClient control(MessageServer message)
{
    // DEBUG
    if (DEBUG) 
    {
        std::cout << "\nAngle: "    << std::to_string(message.angle)
                  << "\nTrackPos: " << std::to_string(message.trackPos)
                  << std::endl;
    }
    
    // Control struct
    MessageClient control;

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
    
    // DO NOT CHANGE
    control.clutch = 0.0;
    control.focus  = 0.0;
    control.gear   = 1;
    control.meta   = false;

    return control;
}