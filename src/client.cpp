#include "headers/client.hpp"
#include <iostream>

Client::Client(std::uint16_t port, std::string ip) : m_port(port), m_ip(ip)
{
    if ((m_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::print(stderr, "Socket creation error\n");
        exit(EXIT_FAILURE);
    }

    m_serverAddress.sin_family = AF_INET;
    m_serverAddress.sin_port = htons(m_port);

    if (inet_pton(AF_INET, "127.0.0.1", &m_serverAddress.sin_addr) <= 0)
    {
        std::print(stderr, "Invalid address/ Address not supported\n");
        exit(EXIT_FAILURE);
    }
}

void Client::connect_to_server()
{
    if (connect(m_socket, (struct sockaddr *)&m_serverAddress, sizeof(m_serverAddress)) < 0)
    {
        std::print(stderr, "Connection Failed\n");
        exit(EXIT_FAILURE);
    }

    memset(m_buffer, 0, BUFFER_SIZE_CLIENT);
    read(m_socket, m_buffer, BUFFER_SIZE_CLIENT);
    std::print("{}", m_buffer);

    std::string name;
    std::getline(std::cin, name);
    send(m_socket, name.c_str(), name.length(), 0);

    while (true)
    {
        memset(m_buffer, 0, BUFFER_SIZE_CLIENT);
        int valread = read(m_socket, m_buffer, BUFFER_SIZE_CLIENT);
        std::print("{}", m_buffer);

        std::string input;
        std::getline(std::cin, input);

        if (input == "QUIT")
        {
            send(m_socket, input.c_str(), input.length(), 0);
            break;
        }

        send(m_socket, input.c_str(), input.length(), 0);
    }
}
