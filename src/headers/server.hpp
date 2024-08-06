#pragma once

#include <cstdint>
#include <print>
#include <vector>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

constexpr std::uint16_t BUFFER_SIZE_SERVER = 1024;
constexpr std::uint16_t MAX_CLIENTS = 30;

class Server {
public:
    Server(std::uint16_t port);
    inline ~Server()
    {
        close(m_serverFd);
        close(m_newSocket);
    }

private:
    void start();
    void receive(char* buffer);

private:
    std::uint16_t m_port;
    std::int32_t m_serverFd;
    std::int32_t m_newSocket;
    struct sockaddr_in m_address;
    std::int32_t m_addrlen = sizeof(m_address);
    char m_buffer[BUFFER_SIZE_SERVER] = {0};
    bool m_running;
    std::vector<std::int32_t> client_sockets;
};
