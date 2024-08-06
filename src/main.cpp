#include <cstdint>
#include <cstdio>
#include <print>
#include <cstring>
#include <getopt.h>
#include <string>

#include "headers/server.hpp"

void print_usage(void)
{
    std::printf("Usage: Teteko [server|client] [--port=PORT] [--ip=IP]\n");
}

std::int32_t main(std::int32_t argc, char* argv[])
{
    if (argc < 2) {
        print_usage();
        return 1;
    }

    std::string mode = argv[1];
    std::string port;
    std::string ip;

    static struct option long_options[] = {
        {"port", required_argument, 0, 'p'},
        {"ip", required_argument, 0, 'i'},
        {0, 0, 0, 0}
    };

    std::int32_t opt;
    std::int32_t option_index = 0;

    while ((opt = getopt_long(argc, argv, "p:i:", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'p':
                port = optarg;
                break;
            case 'i':
                ip = optarg;
                break;
            default:
                print_usage();
                return 1;
        }
    }

    if (mode == "server") {
        if (port.empty()) {
            std::print(stderr, "Error: Port is required for server mode\n");
            print_usage();
            return 1;
        }
        int port_num = std::stoi(port);
        if (port_num < 0 || port_num > 65535) {
            std::print(stderr, "Error: Invalid port number\n");
            return 1;
        }
        std::print("Running in server mode on port {}\n", port_num);
        Server server{static_cast<std::uint16_t>(port_num)};
    } else if (mode == "client") {
        if (port.empty() || ip.empty()) {
            std::print(stderr, "Error: Both port and IP are required for client mode\n");
            print_usage();
            return 1;
        }
        std::print("Running in client mode, connecting to {} on port {}\n", ip, port);
    } else {
        std::print(stderr, "Error: Invalid mode. Use 'server' or 'client'\n");
        print_usage();
        return 1;
    }

    return 0;
}
