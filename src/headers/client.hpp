#pragma once

#include <arpa/inet.h>
#include <cstdint>
#include <cstring>
#include <print>
#include <sys/socket.h>
#include <unistd.h>

constexpr std::uint16_t BUFFER_SIZE_CLIENT = 1024;

class Client
{
  public:
    Client(std::uint16_t port, std::string ip);
    inline ~Client()
    {
        close(m_socket);
    }

  public:
    void connect_to_server();

  private:
    std::int32_t m_socket = 0;
    struct sockaddr_in m_serverAddress;
    char m_buffer[BUFFER_SIZE_CLIENT] = {0};
    std::int16_t m_port;
    std::string m_ip;
};
