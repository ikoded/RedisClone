#include "../include/Connection.h"

/*

Helper for HTTP response to frontend (future)

*/
std::string prepare_http_response(){
    std::string http = 
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: application/json; charset=utf-8\r\n"
    "Content-Length: 64\r\n"
    "Connection: keep-alive\r\n"
    "\r\n"
    "{'status':'success','message':'Server is running','port':8080}";

    return http;
}
/*

Start TCP Domain, poll, accept, and receive

*/
void Connection::start_tcp_domain(){
    Redis redis;
    // set up polling
    std::vector<pollfd> watchlist;

    std::cout << "Creating socket..." << std::endl;
    // socket fd with AF_INET and protocol TCP (auto chosen with 0 set)
    int server_fd = socket(AF_INET,SOCK_STREAM,0);
    if(server_fd==-1){
        std::cerr << "Server socket creation failed." << std::endl;
        return;
    }

    // set socket to be reused easier when server goes down
    int opt = 1;
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0){
        std::cerr << "setsockopt SO_REUSEADDR error." << std::endl;
        close(server_fd);
    }

    // set server_fd as listener poll
    pollfd listening_pfd;
    listening_pfd.fd = server_fd;
    listening_pfd.events = POLLIN; // alert when client comes in
    listening_pfd.revents = 0;
    watchlist.push_back(listening_pfd);

    std::cout << "Server binding to socket..." << std::endl;
    if(bind(server_fd, reinterpret_cast<sockaddr*>(&IPv4Addresses),sizeof(IPv4Addresses)) == -1){
        std::cerr << "Server binding to socket failed." << std::endl;
        close(server_fd);
        return;
    }

    std::cout << "Server listening on socket..." << std::endl;
    if(listen(server_fd,10) == -1){
        std::cerr << "Server listening on socket failed." << std::endl;
        close(server_fd);
        return;
    }

    // make server socket non blocking for polling
    if(!socket_non_blocking_helper(server_fd)){
        std::cerr << "Error setting non blocking server fd " << server_fd << " status flags." << std::endl;
        close(server_fd);
        return;
    }

    while(true){
        // block until an event happens
        int activity = poll(watchlist.data(),watchlist.size(),-1);

        if(activity < 0){
            std::cerr << "Poll error occured." << std::endl;
            break;
        }

        // if server socket gets a client received 
        if(watchlist[0].revents & POLLIN){
            // accept client, returns immediately with polls help
            int client_fd = accept(server_fd,nullptr,nullptr);

            // make sure client properly connected
            if(client_fd >= 0){
                // make client socket non blocking
                if(!socket_non_blocking_helper(client_fd)){
                    std::cerr << "Error setting non blocking client fd " << client_fd << "." << std::endl;
                }else{
                    // set up client pfd
                    pollfd client_pfd;
                    client_pfd.fd = client_fd;
                    client_pfd.events = POLLIN; // alert when sends a query
                    client_pfd.revents = 0;
                    watchlist.push_back(client_pfd);

                    std::cout << "Server accepted client " << client_fd << "." << std::endl;
                }
            }else{
                std::cerr << "Server " << server_fd << " failed accepting client " << client_fd << "." << std::endl;
            }
            
        }

        // loop through clients recieved (skipping server pollfd at index 0)
        for(size_t i = 1; i < watchlist.size();){
            // check if client in loop actually sent something, if not continue and increase i
            if(watchlist[i].revents & POLLIN){
                // read the data the client sent (query)
                std::string query = read_client_data(watchlist[i].fd);

                // if query is blank then something with receiving failed
                if(query == ""){
                    std::cerr << "Client " << watchlist[i].fd << " disconnected or errored." << std::endl;
                }else{
                    std::cout << "Recieved data from " << watchlist[i].fd << "." << std::endl;
                    // parse query
                    std::vector<std::string> parsed = Redis::parse_query(query);
                    // if parsed query returns with size 0, query was wrong
                    if(parsed.size()==0){
                        std::cerr << "Query " << query << " incorrect from client " << watchlist[i].fd << "." << std::endl;
                    }else{
                        // process query request
                        redis.process_query(parsed);
                        // debug
                        redis.print_dict();
                    }
                }
                // closes and erases for now, accepts single query then removes client
                // REMOVE IN FUTURE
                close(watchlist[i].fd);
                watchlist.erase(watchlist.begin()+i);
            }else{
                // client stays in list until it sends data
                ++i;
            }
        }
        
    }

    close(server_fd);
}

/*

Reading client data that comes in

*/
std::string Connection::read_client_data(int client_fd){
    std::string received_string;
    char buffer[1024] = {0};

    ssize_t received_bytes;
    // keep receiving bytes in case not all sent right away
    while((received_bytes = recv(client_fd,buffer,(sizeof(buffer)-1),0)) > 0){
        // recieving and handling null terminator automatically
        received_string.append(buffer,received_bytes);
    }

    if(received_bytes==-1){
        std::cerr << "Error receiving bytes from client " << client_fd << "." << std::endl;
        return "";
    }

    return received_string;
}

/*

Helper function for non blocking fd

*/
bool Connection::socket_non_blocking_helper(int fd){
    // get status flags of fd
    int flags = fcntl(fd, F_GETFL, 0);
    if(flags == -1){
        std::cerr << "Could not get fd " << fd << " status flags." << std::endl;
        return false;
    }
    // try to set flags of fd for non blocking operations
    if(fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1){
        std::cerr << "Could not set the status flags for fd " << fd << "." << std::endl;
        return false;
    }

    return true;
}

/*

Connection Initializer

*/
Connection::Connection(){
    // initialize the struct for sockaddr_in
    IPv4Addresses.sin_family = AF_INET;
    IPv4Addresses.sin_port = htons(8080);
    // map ip to binary
    inet_pton(AF_INET, "127.0.0.1", &IPv4Addresses.sin_addr);
}