#include <iostream>
#include <cstring>
#include <cstdlib>

#include "parser.hpp"


// Helper
float get_key(const char* message, const char* key) {
    char* found = std::strstr((char*)message, key);
    if (found) {
        return std::strtof(found + std::strlen(key), nullptr);
    }
    return 0.0f;
}


MessageServer parse_message_from_server(const char* message)
{
    // DEBUG
    std::cout << "Message to parse: " << message << std::endl;

    // Initialize message struct
    MessageServer message_parsed;

    message_parsed.angle    = get_key(message, "(angle ");
    message_parsed.trackPos = get_key(message, "(trackPos ");

    message_parsed.speedX   = get_key(message, "(speedX ");
    message_parsed.speedY   = get_key(message, "(speedY ");
    message_parsed.speedZ   = get_key(message, "(speedZ ");
    
    // DEBUG
    std::cout << "Message Parsed:\n"
              << "  Angle:" << message_parsed.angle
              << "  Track Pos:" << message_parsed.trackPos
              << "  Speed X:" << message_parsed.speedX
              << "  Speed Y:" << message_parsed.speedY
              << "  Speed Z:" << message_parsed.speedZ
              << std::endl;

    return message_parsed;
}