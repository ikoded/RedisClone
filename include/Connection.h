#pragma once
#include <sys/socket.h> // for sockets
#include <netinet/in.h> // for sockaddr_in
#include <arpa/inet.h> // for inet_pton
#include <unistd.h> // for closing socket
#include <iostream>
#include <fcntl.h> // Non blocking socket
#include <sys/poll.h> // For polling

#include "Redis.h"

class Connection {
    public:
        // Start TCP Domain, poll, accept, and receive
        void start_tcp_domain();
        // Reading client data that comes in
        std::string read_client_data(int clinet_fd);
        // Helper function for non blocking fd
        bool socket_non_blocking_helper(int fd);
        // Connection Initializer
        Connection();
    private:
        struct sockaddr_in IPv4Addresses{};
};