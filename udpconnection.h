#ifndef UDPCONNECTION_H
#define UDPCONNECTION_H
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
class UDPConnection{
    private:
    int server_socket;
    socklen_t clientlen;
    int recieve_l,send_l;
    struct sockaddr_in client_address,server_address;
    char buffer[300];
    public:
    UDPConnection(char* host, int port);
    ~UDPConnection();
    void listen();
};


#endif // UDPCONNECTION_H
