/*udpconnection.cpp
* auhor: Jakub Kontrik
* login: xkontr02
* brief: udp connection for ipk calculator
*/
#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include "parser.h"
#include "udpconnection.h"

UDPConnection::UDPConnection(const char* host, int port){
	if ((this->server_socket = socket(AF_INET, SOCK_DGRAM, 0)) <= 0){
		perror("ERROR: socket");
		exit(EXIT_FAILURE);
	}
    // after closing the socket, is ready to be reused
    int optval = 1;
    setsockopt(this->server_socket, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));

    bzero((char *) &(this->server_address), sizeof(this->server_address));
    this->server_address.sin_family = AF_INET;
    // convert ip address from text to binary form
    if(inet_aton(host, &(this->server_address.sin_addr)) == 0){
        perror("ERROR: inet_aton");
        close(this->server_socket);
        exit(EXIT_FAILURE);
    }
    this->server_address.sin_port = htons((unsigned short)port);

    if (bind(this->server_socket, (struct sockaddr *) &(this->server_address), sizeof(this->server_address)) < 0) 
    {
        perror("ERROR: binding");
        close(this->server_socket);
        exit(EXIT_FAILURE);
    }
}


// fuction for handling client requests
void UDPConnection::listen(){
    // buffer is set to size 300 just ist case valid message is not longer than 255 characters
    bzero(this->buffer,UDP_BUFFER_SIZE);
    // sigint "handler" for udp is just end of loop
    while(!sigint_flag){
        bzero(this->buffer,UDP_BUFFER_SIZE);
        this->clientlen = sizeof(this->client_address);
        // recieve message from client and store it in buffer
        // buffer size minus 1 because of the null terminator
        if (recvfrom(this->server_socket, this->buffer, UDP_BUFFER_SIZE-1, 0, 
            (struct sockaddr *)&(this->client_address), &(this->clientlen)) < 0){
            perror("ERROR: recvfrom:");
            continue;
        }
        try{
        // check if opcode is correct
        if (this->buffer[0] != '\0'){
            throw -1;
        }
        std::string message((this->buffer)+2);
        // slice the message to the length specified in the second byte
        message = message.substr(0, this->buffer[1]);
        std::cout << "Recieved: " << message << std::endl;
        // parse the message
        Parser parser(message);
        parser.parse();

        message.clear();
        message = std::to_string(parser.result);
        bzero(this->buffer,UDP_BUFFER_SIZE);
        // set valid response
        this->buffer[0] = '\1';
        this->buffer[1] = '\0';
        this->buffer[2]=(char)message.length();
        memcpy((this->buffer) + 3, message.c_str(), message.length());
        }
        // catchblock to catch any error
        catch(...){
            std::cout << "Error with sent message" << std::endl;
            bzero(this->buffer,UDP_BUFFER_SIZE);
            // set invalid response
            this->buffer[0] = '\1';
            this->buffer[1] = '\1';
            this->buffer[2] = 23;
            memcpy((this->buffer) + 3, "Error with sent message", 23);
        }
        if (sendto(this->server_socket, this->buffer, strlen(buffer+3)+3, 0, 
            (struct sockaddr *) &(this->client_address), this->clientlen) < 0){
            perror("ERROR: sendto:");
        }
    }
}



