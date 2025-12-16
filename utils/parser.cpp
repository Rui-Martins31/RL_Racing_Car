#include <iostream>
#include <cstring>
#include <cstdlib>

// Custom scripts
#include "parser.hpp"

// Gobals
#define DEBUG true

// Helper
float get_key(const char* message, const char* key) {
    char* found = std::strstr((char*)message, key);
    if (found) {
        return std::strtof(found + std::strlen(key), nullptr);
    }
    return 0.0f;
}

// Main methods
MessageServer parse_message_from_server(const char* message)
{
    // DEBUG
    // if (DEBUG) { std::cout << "\nMessage to parse: " << message << std::endl; }

    // Initialize message struct
    MessageServer message_parsed;

    message_parsed.angle    = get_key(message, "(angle ");
    message_parsed.trackPos = get_key(message, "(trackPos ");

    message_parsed.speedX   = get_key(message, "(speedX ");
    message_parsed.speedY   = get_key(message, "(speedY ");
    message_parsed.speedZ   = get_key(message, "(speedZ ");
    
    // DEBUG
    if (DEBUG)
    {
        std::cout << "Message Parsed:"
                << "\n  Angle:" << message_parsed.angle
                << "\n  Track Pos:" << message_parsed.trackPos
                << "\n  Speed X:" << message_parsed.speedX
                << "\n  Speed Y:" << message_parsed.speedY
                << "\n  Speed Z:" << message_parsed.speedZ
                << std::endl;
    }

    return message_parsed;
}

std::string parse_message_from_client(MessageClient control_message)
{
    // Message to send
    std::string message = "";
    
    message += "(accel "  + std::to_string(control_message.accel)  + ")";
    message += "(brake "  + std::to_string(control_message.brake)  + ")";
    message += "(clutch " + std::to_string(control_message.clutch) + ")";
    message += "(gear "   + std::to_string(control_message.gear)   + ")";
    message += "(steer "  + std::to_string(control_message.steer)  + ")";
    message += "(focus "  + std::to_string(control_message.focus)  + ")";
    message += "(meta "   + std::to_string(control_message.meta)   + ")";
    message += "\0";

    // DEBUG
    if (DEBUG) { std::cout << "Message Control:\n" << message << std::endl; }

    return message;
}