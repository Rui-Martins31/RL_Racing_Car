#include <stdio.h>

#include "control.hpp"

MessageClient control(MessageServer message)
{
    // Control struct
    MessageClient control;

    // Placeholder
    control.accel = 1.0;
    control.brake = 0.0;
    control.clutch = 0.0;
    control.focus = 0.0;
    control.gear = 1;
    control.meta = false;
    control.steering = 0.0;

    return control;
}