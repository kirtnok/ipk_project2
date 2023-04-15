#include "udpconnection.h"
#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include "parser.h"


UDPConnection::UDPConnection(char* host, int port){
	if ((this->server_socket = socket(AF_INET, SOCK_DGRAM, 0)) <= 0)
	{
		perror("ERROR: socket");
		exit(EXIT_FAILURE);
	}
    /* potlaceni defaultniho chovani rezervace portu ukonceni aplikace */ 
    int optval = 1;
    setsockopt(this->server_socket, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));

    /* adresa serveru, potrebuje pro prirazeni pozadovaneho portu */
    bzero((char *) &(this->server_address), sizeof(this->server_address));
    this->server_address.sin_family = AF_INET;
    if(inet_aton(host, &(this->server_address.sin_addr)) == 0){
        perror("ERROR: inet_aton");
        exit(EXIT_FAILURE);
    }
    this->server_address.sin_port = htons((unsigned short)port);
	
    if (bind(this->server_socket, (struct sockaddr *) &(this->server_address), sizeof(this->server_address)) < 0) 
    {
        perror("ERROR: binding");
        exit(EXIT_FAILURE);
    }
    
}

UDPConnection::~UDPConnection(){
    close(this->server_socket);
}

void UDPConnection::listen(){
    bzero(this->buffer,300);
    while(1){
        bzero(this->buffer,300);
        this->clientlen = sizeof(this->client_address);
        this->send_l = recvfrom(this->server_socket, this->buffer, 300, 0, (struct sockaddr *) &(this->client_address), &(this->clientlen));
        if (this->send_l < 0) 
            perror("ERROR: recvfrom:");
        this->buffer[299] = '\0';
        std::string message((this->buffer)+2);
        std::cout << message << std::endl;
        Parser parser(message);
        try{
            parser.parse();
        
        message.clear();
        message = std::to_string(parser.result);

        bzero(this->buffer,300);
        this->buffer[0] = '1';
        this->buffer[1] = '\0';
        this->buffer[3]=(char)message.length();
        memcpy((this->buffer) + 3, message.c_str(), message.length());
        std::cout << (this->buffer) + 3 << std::endl;
        }
        catch(...){
            bzero(this->buffer,300);
            this->buffer[0] = '1';
            this->buffer[1] = '\1';
            this->buffer[3] = 23;
            memcpy((this->buffer) + 3, "Error with sent message", 23);
        }
        this->recieve_l = sendto(this->server_socket, this->buffer, 300, 0, (struct sockaddr *) &(this->client_address), this->clientlen);
        if (this->recieve_l < 0) 
            perror("ERROR: sendto:");
    }
}



