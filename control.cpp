#include <iostream>

// Custom scripts
#include "control.hpp"

// Globals
#define DEBUG true

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
    control.accel = 0.25;
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