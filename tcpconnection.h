#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>

extern bool sigint_flag;
class TCPConnection{
    private:
    int welcome_socket;
    struct sockaddr_in server_address;
    struct sockaddr *comm_addr;
    socklen_t comm_addr_size;


    public:
    std::vector<int> client_sockets;
    TCPConnection(const char* host, int port);
    void listen_tcp();
};
void *handle_connection(void *arg);

#endif // TCPCONNECTION_H