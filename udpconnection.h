/*udpconnection.h
* auhor: Jakub Kontrik
* login: xkontr02
*/
#ifndef UDPCONNECTION_H
#define UDPCONNECTION_H
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
// buffer set to 300 for better work
#define UDP_BUFFER_SIZE 300
extern bool sigint_flag;
// class for handling udp connections
class UDPConnection{
    private:
    socklen_t clientlen;
    struct sockaddr_in client_address,server_address;
    char buffer[UDP_BUFFER_SIZE];

    public:
    int server_socket;
    UDPConnection(const char* host, int port);
    void listen();
};


#endif // UDPCONNECTION_H
