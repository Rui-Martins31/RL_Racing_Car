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
}; // This vars are enough for now



// Methods

// Parse a given message and return an array with the data
MessageServer parse_message_from_server(const char* message);