#include "headers/server.hpp"
#include <mutex>
#include <thread>

std::mutex g_clientMutex;

Server::Server(std::uint16_t port) : m_port(port)
{
    this->start();

    std::print("Server is running. Waiting for connections...\n");

    while (true)
    {
        if ((m_newSocket = accept(m_serverFd, (struct sockaddr *)&m_address, (socklen_t *)&m_addrlen)) < 0)
        {
            perror("accept");
            continue;
        }

        std::print("New connection, socket fd is {} , IP is : {} , port : {}\n", m_newSocket,
                   inet_ntoa(m_address.sin_addr), ntohs(m_address.sin_port));

        const char *welcome_message = "Welcome to the server. Please enter your name: ";
        send(m_newSocket, welcome_message, strlen(welcome_message), 0);

        memset(m_buffer, 0, BUFFER_SIZE_SERVER);
        int valread = read(m_newSocket, m_buffer, BUFFER_SIZE_SERVER);
        if (valread > 0)
        {
            std::string client_name(m_buffer);
            client_name.erase(std::remove(client_name.begin(), client_name.end(), '\n'), client_name.end());

            if (client_info.size() < MAX_CLIENTS)
            {
                {
                    std::lock_guard<std::mutex> lock(g_clientMutex);
                    client_info.push_back(std::make_pair(m_newSocket, client_name));
                }
                std::print("Adding to list of sockets as {}, name: {}\n", client_info.size() - 1, client_name);

                std::string confirm_message =
                    "Hello, " + client_name + "! You're now connected. Type your messages or 'QUIT' to exit.\n";
                send(m_newSocket, confirm_message.c_str(), confirm_message.length(), 0);

                std::thread client_thread(&Server::handle_client, m_newSocket, std::cref(client_name),
                                          std::ref(client_info));
                client_thread.detach();
            }
            else
            {
                const char *max_clients_message = "Server full. Try again later.\n";
                send(m_newSocket, max_clients_message, strlen(max_clients_message), 0);
                close(m_newSocket);
            }
        }
    }
}

void Server::start()
{
    std::int32_t opt = 1;
    if ((m_serverFd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        std::print(stderr, "socket failed");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(m_serverFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        std::print(stderr, "setsockopt");
        exit(EXIT_FAILURE);
    }
    m_address.sin_family = AF_INET;
    m_address.sin_addr.s_addr = INADDR_ANY;
    m_address.sin_port = htons(m_port);

    if (bind(m_serverFd, (struct sockaddr *)&m_address, sizeof(m_address)) < 0)
    {
        std::print(stderr, "bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(m_serverFd, 3) < 0)
    {
        std::print(stderr, "listen");
        exit(EXIT_FAILURE);
    }
}

void Server::handle_client(std::int32_t client_socket, const std::string &client_name,
                           std::vector<std::pair<std::int32_t, std::string>> &client_info)
{
    char buffer[BUFFER_SIZE_SERVER] = {0};

    while (true)
    {
        memset(buffer, 0, BUFFER_SIZE_SERVER);
        int valread = read(client_socket, buffer, BUFFER_SIZE_SERVER);

        if (valread <= 0)
        {
            std::lock_guard<std::mutex> lock(g_clientMutex);
            auto it = std::find_if(client_info.begin(), client_info.end(),
                                   [client_socket](const std::pair<std::int32_t, std::string> &element) {
                                       return element.first == client_socket;
                                   });

            if (it != client_info.end())
            {
                std::print("Client disconnected: {} (socket {})\n", it->second, it->first);
                client_info.erase(it);
            }
            close(client_socket);
            break;
        }
        else
        {
            std::string message(buffer);
            message.erase(std::remove(message.begin(), message.end(), '\n'), message.end());
            std::print("Received message from {}, (socket {}): {}\n", client_name, client_socket, message);

            if (message == "QUIT")
            {
                std::string goodbye_message = "Goodbye, " + client_name + "!\n";
                send(client_socket, goodbye_message.c_str(), goodbye_message.length(), 0);

                std::lock_guard<std::mutex> lock(g_clientMutex);
                auto it = std::find_if(client_info.begin(), client_info.end(),
                                       [client_socket](const std::pair<std::int32_t, std::string> &element) {
                                           return element.first == client_socket;
                                       });

                if (it != client_info.end())
                {
                    std::print("Client quit: {}  (socket {})\n", it->second, it->first);
                    client_info.erase(it);
                }
                close(client_socket);
                break;
            }
            else
            {
                std::string response = "Server received: " + message + "\n";
                send(client_socket, response.c_str(), response.length(), 0);
            }
        }
    }
}
