#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <array>

// ==============================
// TORCS → CLIENT MESSAGE
// ==============================
struct MessageServer
{
    float angle;
    float trackPos;

    float speedX;
    float speedY;
    float speedZ;

    float distRaced;
    float lastLapTime;

    // Track range sensors (19 values)
    std::array<float, 19> track;
    
    // NOVO: Dano do carro
    float damage;
};

// ==============================
// CLIENT → TORCS MESSAGE
// ==============================
struct MessageClient
{
    float accel;
    float brake;
    float clutch;
    int   gear;
    float steer;
    float focus;
    bool  meta;
};

// ==============================
// PARSER FUNCTIONS
// ==============================
MessageServer parse_message_from_server(const char* message);
std::string parse_message_from_client(MessageClient control_message);

#endif // PARSER_HPP