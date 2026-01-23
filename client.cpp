#include <iostream>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <signal.h>
#include <cerrno>

#include "utils/parser.hpp"
#include "control.hpp"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 3001
#define BUFFER_SIZE 4096
#define SOCKET_TIMEOUT 3

volatile bool running = true;

void signal_handler(int signum)
{
    std::cout << "\n[CLIENT] Shutdown signal received..." << std::endl;
    running = false;
}

bool handshake(int sockfd, sockaddr_in& servaddr)
{
    std::string init = "SCR init";
    sendto(sockfd, init.c_str(), init.size(), 0,
           (sockaddr*)&servaddr, sizeof(servaddr));

    char buf[BUFFER_SIZE];
    int n = recv(sockfd, buf, sizeof(buf) - 1, 0);
    if (n <= 0) return false;

    buf[n] = '\0';
    return std::strcmp(buf, "***identified***") == 0;
}

int main()
{
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    std::cout << "========================================" << std::endl;
    std::cout << "Starting TORCS client (PPO)" << std::endl;
    std::cout << "========================================" << std::endl;

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "[CLIENT] Error creating socket" << std::endl;
        return 1;
    }

    sockaddr_in servaddr{};
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);
    
    if (inet_pton(AF_INET, SERVER_IP, &servaddr.sin_addr) <= 0) {
        std::cerr << "[CLIENT] Invalid server IP address" << std::endl;
        close(sockfd);
        return 1;
    }

    timeval tv{SOCKET_TIMEOUT, 0};
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    std::cout << "[CLIENT] Connecting to TORCS server at " 
              << SERVER_IP << ":" << SERVER_PORT << "..." << std::endl;

    int handshake_attempts = 0;
    while (!handshake(sockfd, servaddr) && running)
    {
        std::cout << "." << std::flush;
        handshake_attempts++;
        
        if (handshake_attempts > 30) {
            std::cerr << "\n[CLIENT] Handshake failed after 30 attempts" << std::endl;
            close(sockfd);
            return 1;
        }
        
        sleep(1);
    }

    if (!running) {
        std::cout << "[CLIENT] Interrupted during handshake" << std::endl;
        close(sockfd);
        return 0;
    }

    std::cout << "\n[CLIENT] Connected to TORCS server!" << std::endl;
    std::cout << "========================================" << std::endl;

    char message[BUFFER_SIZE];
    int episode_cycles = 1;
    int total_episodes = 0;
    int total_steps = 0;

    while (running)
    {
        int n = recv(sockfd, message, sizeof(message) - 1, 0);
        
        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            }
            std::cerr << "[CLIENT] Recv error: " << strerror(errno) << std::endl;
            break;
        }
        
        if (n == 0) {
            std::cout << "[CLIENT] Server closed connection" << std::endl;
            break;
        }
        
        message[n] = '\0';

        if (std::strcmp(message, "***restart***") == 0)
        {
            std::cout << "[CLIENT] Episode " << total_episodes
                      << " restart (lasted " << episode_cycles << " steps)" << std::endl;

            // MANDATORY: re-handshake with TORCS
            while (!handshake(sockfd, servaddr))
            {
                std::cout << "." << std::flush;
            }

            episode_cycles = 1;
            total_episodes++;
            continue;
        }

        if (std::strcmp(message, "***shutdown***") == 0)
        {
            std::cout << "[CLIENT] Shutdown requested by server" << std::endl;
            running = false;
            break;
        }

        MessageServer obs = parse_message_from_server(message);
        MessageClient act = step(episode_cycles, obs);

        std::string out = parse_message_from_client(act);
        ssize_t sent = sendto(sockfd, out.c_str(), out.size(), 0,
                              (sockaddr*)&servaddr, sizeof(servaddr));
        
        if (sent < 0) {
            std::cerr << "[CLIENT] Send error: " << strerror(errno) << std::endl;
            break;
        }

        episode_cycles++;
        total_steps++;
        
        if (total_steps % 1000 == 0) {
            std::cout << "[CLIENT] Total steps: " << total_steps 
                      << ", Episodes: " << total_episodes << std::endl;
        }
    }

    std::cout << "\n========================================" << std::endl;
    std::cout << "[CLIENT] Shutting down..." << std::endl;
    std::cout << "Total episodes: " << total_episodes << std::endl;
    std::cout << "Total steps: " << total_steps << std::endl;
    std::cout << "========================================" << std::endl;

    cleanup_ppo();
    close(sockfd);
    
    return 0;
}