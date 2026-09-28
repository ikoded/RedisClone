#include "../include/Connection.h"

/*

Basic TCP Domain started and accepting

*/
void Connection::start_tcp_domain(){
    Redis redis;

    int client_count = 0;
    std::cout << "Creating socket..." << std::endl;

    // socket fd with AF_INET and protocol TCP (auto chosen with 0 set)
    int server_fd = socket(AF_INET,SOCK_STREAM,0);
    if(server_fd==-1){
        std::cerr << "Server socket creation failed." << std::endl;
        return;
    }

    std::cout << "Server binding to socket..." << std::endl;
    if(bind(server_fd, reinterpret_cast<sockaddr*>(&IPv4Addresses),sizeof(IPv4Addresses)) == -1){
        std::cerr << "Server binding to socket failed." << std::endl;
        return;
    }

    std::cout << "Server listening on socket..." << std::endl;
    if(listen(server_fd,10) == -1){
        std::cerr << "Server listening on socket failed." << std::endl;
        return;
    }

    while(client_count < 10){
        if(client_count == 0){ // only run on first iteration
            std::cout << "Server " << server_fd << " accepting.." << std::endl;
        }
        int client_fd = accept(server_fd,nullptr,nullptr);

        if(client_fd == -1){
            std::cerr << "Server " << server_fd << " failed accepting client " << client_fd << std::endl;
        }
        std::cout << "Server " << server_fd << " accepted " << " client " << client_fd << "." << std::endl;

        std::string query = read_client_data(client_fd);

        client_count++;

        std::vector<std::string> parsed = Redis::parse_query(query);
        if(parsed.size()==0){
            std::cerr << "Query " << query << " failed, exiting." << std::endl;
            close(client_fd);
            break;
        }
        for(std::string word : parsed){
            std::cout << "- " << word << std::endl;
        }
    }

    close(server_fd);
}

/*

Reading client data that comes in

*/
std::string Connection::read_client_data(int client_fd){
    std::string received_string;
    char buffer[255] = {0};

    ssize_t received_bytes;
    while((received_bytes = recv(client_fd,buffer,(sizeof(buffer)-1),0)) > 0){
        // recieving and handling null terminator automatically
        received_string.append(buffer,received_bytes);
    }

    if(received_bytes==-1){
        std::cerr << "Error receiving bytes from client " << client_fd << "." << std::endl;
        return "";
    }

    close(client_fd);

    return received_string;
}

Connection::Connection(){
    // initialize the struct for sockaddr_in
    IPv4Addresses.sin_family = AF_INET;
    IPv4Addresses.sin_port = htons(8080);

    inet_pton(AF_INET, "127.0.0.1", &IPv4Addresses.sin_addr);
}