#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <cmath>
#include <arpa/inet.h> // For sockets (Linux/macOS)
#include <unistd.h>
#include <sys/socket.h>

// Custom scripts
#include "utils/parser.hpp"
#include "control.hpp"

// Globals
const char* SERVER_IP    = "127.0.0.1";
const int SERVER_PORT    = 3001;
const int SOCKET_TIMEOUT = 3;
const int BUFFER_SIZE    = 4096;
const int MAX_STEPS      = 0;

bool handshake(int sockfd, sockaddr_in servaddr)
{
    std::cout << "Sending..." << std::endl;
    // Send
    std::string init_str = "SCR init";
    sendto(sockfd, init_str.c_str(), init_str.length(), 0, 
            (const struct sockaddr *) &servaddr, sizeof(servaddr));

    std::cout << "Receiving..." << std::endl;
    // Receive
    char message[BUFFER_SIZE];
    int n_chars = recv(sockfd, message, sizeof(message), 0);

    std::cout << "Returning..." << std::endl;
    std::cout << message << std::endl;
    
    if (strcmp(message, "***identified***") == 0 /*|| strcmp(message, "***restart***") == 0*/) return true;
    else return false;
}

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

    struct timeval timeout;
    timeout.tv_sec = SOCKET_TIMEOUT;

    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("setsockopt failed");
        close(sockfd);
        return 1;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &servaddr.sin_addr) <= 0) {
        perror("Invalid address");
        return -1;
    }

    // Intial handshake
    while (!handshake(sockfd, servaddr)) {std::cout << ".";}


    // DEBUG
    std::cout << "Connected to TORCS server..." << std::endl;
    std::cout << "Receveiving feedback from TORCS server...\n" << std::endl;

    char message[BUFFER_SIZE];
    socklen_t len = sizeof(servaddr);

    // Loop
    while (true)
    {
        // Receive message
        int n_chars = recv(sockfd, message, sizeof(message), 0);

        if (strcmp(message, "***restart***") == 0) 
        {
            std::cout << "Restarting..." << std::endl;

            // Restart
            while (!handshake(sockfd, servaddr)) {std::cout << ".";}
            continue;
        }
        message[n_chars] = '\0';

        // DEBUG
        // std::cout << "Message from client: " << message << std::endl;

        // Parse server message
        MessageServer message_parsed = parse_message_from_server(message);

        // Control
        MessageClient message_control = control(message_parsed); 

        // Create client message
        std::string response = parse_message_from_client(message_control);

        // Send control message
        sendto(sockfd, response.c_str(), response.length(), 0,
                (const struct sockaddr *) &servaddr, sizeof(servaddr));
    }

    // Close socket
    close(sockfd);

    return 0;
}