#include "headers/server.hpp"
#include <cstdio>

Server::Server(std::uint16_t port) : m_port(port) {
    this->start();

    std::print("Server is running. Waiting for connections...\n");

        while(true) {
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(m_serverFd, &readfds);
            int max_sd = m_serverFd;

            // Add child sockets to set
            for (int sd : client_sockets) {
                FD_SET(sd, &readfds);
                if (sd > max_sd)
                    max_sd = sd;
            }

            // Wait for an activity on one of the sockets
            int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
            if ((activity < 0) && (errno != EINTR)) {
                std::print(stderr, "select error\n");
            }

            // If something happened on the master socket, then it's an incoming connection
            if (FD_ISSET(m_serverFd, &readfds)) {
                if ((m_newSocket = accept(m_serverFd, (struct sockaddr *)&m_address, (socklen_t*)&m_addrlen)) < 0) {
                    std::print(stderr, "accept");
                    exit(EXIT_FAILURE);
                }

                std::print("New connection, socket fd is {} , IP is : {} , port : {}\n", m_newSocket, inet_ntoa(m_address.sin_addr), ntohs(m_address.sin_port));

                // Send welcome message
                std::string welcome_message = "Welcome to the server. Please enter your name: ";
                send(m_newSocket, welcome_message.c_str(), welcome_message.size(), 0);

                // Add new socket to array of sockets
                if (client_sockets.size() < MAX_CLIENTS) {
                    client_sockets.push_back(m_newSocket);
                    std::print("Adding to list of sockets as {} \n", client_sockets.size() - 1);
                } else {
                    std::string max_clients_message = "Server full. Try again later.\n";
                    send(m_newSocket, max_clients_message.c_str(), max_clients_message.size(), 0);
                    close(m_newSocket);
                }
            }

            // Else it's some IO operation on some other socket
            for (auto it = client_sockets.begin(); it != client_sockets.end();) {
                int sd = *it;
                if (FD_ISSET(sd, &readfds)) {
                    // Read the incoming message
                    int valread = read(sd, m_buffer, BUFFER_SIZE_SERVER);
                    if (valread == 0) {
                        // Somebody disconnected, get their details and print
                        getpeername(sd, (struct sockaddr*)&m_address, (socklen_t*)&m_addrlen);
                        std::print("Host disconnected, ip {} , port {}\n", inet_ntoa(m_address.sin_addr), ntohs(m_address.sin_port));

                        // Close the socket and remove from the list
                        close(sd);
                        it = client_sockets.erase(it);
                    } else {
                        // Process the message
                        m_buffer[valread] = '\0';
                        std::string message(m_buffer);
                        std::print("Received message from client {} : {}\n", sd, message);

                        // Respond based on the message content
                        std::string response;
                        if (message.find("NAME:") == 0) {
                            response = "Hello, " + message.substr(5) + "! What would you like to do? (CHAT/QUIT)";
                        } else if (message == "CHAT") {
                            response = "Enter your message:";
                        } else if (message == "QUIT") {
                            response = "Goodbye!";
                            send(sd, response.c_str(), response.length(), 0);
                            close(sd);
                            it = client_sockets.erase(it);
                            continue;
                        } else {
                            response = "Message received. Type CHAT to send another message or QUIT to exit.";
                        }

                        send(sd, response.c_str(), response.length(), 0);
                        ++it;
                    }
                } else {
                    ++it;
                }
            }
        }

}

void Server::start() {
    std::int32_t opt = 1;
    if ((m_serverFd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::print(stderr, "socket failed");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(m_serverFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        std::print(stderr, "setsockopt");
        exit(EXIT_FAILURE);
    }
    m_address.sin_family = AF_INET;
    m_address.sin_addr.s_addr = INADDR_ANY;
    m_address.sin_port = htons(m_port);

    if (bind(m_serverFd, (struct sockaddr *)&m_address, sizeof(m_address)) < 0) {
        std::print(stderr, "bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(m_serverFd, 3) < 0) {
        std::print(stderr, "listen");
        exit(EXIT_FAILURE);
    }
}
