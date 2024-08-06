#pragma once

#include <arpa/inet.h>
#include <cstdint>
#include <cstring>
#include <print>
#include <sys/socket.h>
#include <unistd.h>
#include <utility>
#include <vector>

constexpr std::uint16_t BUFFER_SIZE_SERVER = 1024;
constexpr std::uint16_t MAX_CLIENTS = 30;

class Server
{
  public:
    Server(std::uint16_t port);
    inline ~Server()
    {
        close(m_serverFd);
        close(m_newSocket);
    }

  private:
    void start();
    static void handle_client(std::int32_t client_socket, const std::string &client_name,
                              std::vector<std::pair<std::int32_t, std::string>> &client_info);

  private:
    std::uint16_t m_port;
    std::int32_t m_serverFd;
    std::int32_t m_newSocket;
    struct sockaddr_in m_address;
    std::int32_t m_addrlen = sizeof(m_address);
    char m_buffer[BUFFER_SIZE_SERVER] = {0};
    bool m_running;
    std::vector<std::pair<std::int32_t, std::string>> client_info;
};
