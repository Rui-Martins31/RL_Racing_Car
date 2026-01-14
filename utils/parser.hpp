#ifndef PARSER_HPP
#define PARSER_HPP

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <string>

// Structs

struct MessageServer
{
    // Relative to the center of the track
    float angle;
    float trackPos;
 
    //Speed
    float speedX;
    float speedY;
    float speedZ;

    // Variables for reward computation
    float distRaced;
    float curLapTime;
    float lastLapTime;
}; // This vars are enough for now

struct MessageClient
{
    // Vars to control
    float accel;
    float brake;
    float steer;

    // 
    float clutch;
    int gear;
    float focus;
    bool meta;
};


// Methods

// Parse a given message and returns an array with the data
MessageServer parse_message_from_server(const char* message);

// Parse a MessageClient struct and returns a string
std::string parse_message_from_client(MessageClient control_message);

#endif