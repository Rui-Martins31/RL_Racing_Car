// Custom scripts
#include "parser.hpp"
#include <vector>

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

std::vector<float> get_track_sensors(const char* message) {
    std::vector<float> sensors;
    char* found = std::strstr((char*)message, "(track ");
    if (found) {
        char* ptr = found + std::strlen("(track ");
        char* end;
        while (*ptr && *ptr != ')') {
            float value = std::strtof(ptr, &end);
            if (ptr == end) break;
            sensors.push_back(value);
            ptr = end;
            while (*ptr == ' ') ptr++;
        }
    }
    return sensors;
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

    message_parsed.rpm      = get_key(message, "(rpm ");
    message_parsed.gear     = get_key(message, "(gear ");

    std::vector<float> sensor_readings = get_track_sensors(message);
    message_parsed.sensor_left   = sensor_readings[8];
    message_parsed.sensor_middle = sensor_readings[9];
    message_parsed.sensor_right  = sensor_readings[10];

    message_parsed.distRaced   = get_key(message, "(distRaced ");
    message_parsed.lastLapTime = get_key(message, "(lastLapTime ");
    
    // DEBUG
    if (DEBUG)
    {
        std::cout << std::endl;
        std::cout << "Message Parsed:"
                << "\n  Angle:" << message_parsed.angle
                << "\n  Track Pos:" << message_parsed.trackPos
                << "\n  Speed X:" << message_parsed.speedX
                << "\n  Speed Y:" << message_parsed.speedY
                << "\n  Speed Z:" << message_parsed.speedZ
                << "\n  RPM:" << message_parsed.rpm
                << "\n  Sensor Left:" << message_parsed.sensor_left
                << "\n  Sensor Middle:" << message_parsed.sensor_middle
                << "\n  Sensor Right:" << message_parsed.sensor_right
                << "\n  Dist Raced:" << message_parsed.distRaced
                << "\n  Last Lap Time:" << message_parsed.lastLapTime
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