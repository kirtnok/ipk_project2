/*tcpconnection.h
* auhor: Jakub Kontrik
* login: xkontr02
*/
#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>

#define BUFFER_SIZE 1024
extern bool sigint_flag;
// class for handling tcp connections
class TCPConnection{
    private:
    struct sockaddr_in server_address;
    struct sockaddr *comm_addr;
    socklen_t comm_addr_size;

    public:
    std::vector<int> client_sockets;
    int welcome_socket;
    TCPConnection(const char* host, int port);
    void listen_tcp();

};

void *handle_connection(void *arg);

#endif // TCPCONNECTION_H