#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <cmath>
#include <arpa/inet.h> // For sockets (Linux/macOS)
#include <unistd.h>
#include <sys/socket.h>

// Globals
const char* SERVER_IP = "127.0.0.1";
const int SERVER_PORT = 3001;
const int BUFFER_SIZE = 4096;
const int MAX_STEPS   = 0;

int main()
{
    // DEBUG
    std::cout << "Starting client..." << std::endl;

    // Create UDP Socket
    int sockfd;
    struct sockaddr_in servaddr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &servaddr.sin_addr) <= 0) {
        perror("Invalid address");
        return -1;
    }

    // Intial handshake
    std::string init_str = "SCR(init -1.0 1.0 1.0 0.0 \"PdaddDriver\")";
    sendto(sockfd, init_str.c_str(), init_str.length(), 0, 
           (const struct sockaddr *) &servaddr, sizeof(servaddr));

    // DEBUG
    std::cout << "Connected to TORCS server..." << std::endl;
    std::cout << "Receveiving feddback from TORCS server...\n" << std::endl;

    char message[BUFFER_SIZE];
    socklen_t len = sizeof(servaddr);

    // Loop
    while (true)
    {
        // DEBUG received messages
        recv(sockfd, message, sizeof(message), 0);
        std::cout << "Message from client: " << message << std::endl;

        
    }

    // Close socket
    close(sockfd);

    return 0;
}