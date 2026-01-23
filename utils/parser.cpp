#include "parser.hpp"
#include <cstring>
#include <cstdlib>
#include <string>

// ==============================
// HELPERS
// ==============================
static float get_key(const char* message, const char* key)
{
    const char* found = std::strstr(message, key);
    if (!found) return 0.0f;
    return std::strtof(found + std::strlen(key), nullptr);
}

static void get_array(const char* message,
                      const char* key,
                      float* array,
                      int size)
{
    const char* found = std::strstr(message, key);
    if (!found) {
        for (int i = 0; i < size; i++)
            array[i] = 0.0f;
        return;
    }

    const char* ptr = found + std::strlen(key);

    for (int i = 0; i < size; i++) {
        array[i] = std::strtof(ptr, const_cast<char**>(&ptr));
    }
}

// ==============================
// PARSE SERVER MESSAGE
// ==============================
MessageServer parse_message_from_server(const char* message)
{
    MessageServer msg{};

    msg.angle    = get_key(message, "(angle ");
    msg.trackPos = get_key(message, "(trackPos ");

    msg.speedX = get_key(message, "(speedX ");
    msg.speedY = get_key(message, "(speedY ");
    msg.speedZ = get_key(message, "(speedZ ");

    msg.distRaced   = get_key(message, "(distRaced ");
    msg.lastLapTime = get_key(message, "(lastLapTime ");

    get_array(message, "(track ", msg.track.data(), 19);
    
    // NOVO: Parse damage
    msg.damage = get_key(message, "(damage ");

    return msg;
}

// ==============================
// PARSE CLIENT MESSAGE
// ==============================
std::string parse_message_from_client(MessageClient control)
{
    std::string msg;

    msg += "(accel "  + std::to_string(control.accel)  + ")";
    msg += "(brake "  + std::to_string(control.brake)  + ")";
    msg += "(clutch " + std::to_string(control.clutch) + ")";
    msg += "(gear "   + std::to_string(control.gear)   + ")";
    msg += "(steer "  + std::to_string(control.steer)  + ")";
    msg += "(focus "  + std::to_string(control.focus)  + ")";
    msg += "(meta "   + std::to_string(control.meta)   + ")";

    return msg;
}