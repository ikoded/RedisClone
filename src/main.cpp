#include "../include/Connection.h"
#include "../include/Redis.h"

int main(){
    Connection connection;
    
    connection.start_tcp_domain();

    return 0;
}