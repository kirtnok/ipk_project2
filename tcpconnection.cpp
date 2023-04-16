#include "tcpconnection.h"
#include <sys/socket.h>
#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <vector>
#include <sstream>
#include "parser.h"

#define BUFFER_SIZE 1025


TCPConnection::TCPConnection(const char* host, int port){
    if((this->welcome_socket = socket(AF_INET,SOCK_STREAM,0))<0){
        perror("socket");
        exit(1);
    }

    int optval = 1;
    setsockopt(this->welcome_socket, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(optval));
    memset(&(this->server_address), 0, sizeof(this->server_address));

    this->server_address.sin_family = AF_INET;
    inet_aton(host, &(this->server_address.sin_addr));
    this->server_address.sin_port = htons(port);
    struct sockaddr *adress = (struct sockaddr *) &(this->server_address);
    int adress_len = sizeof(this->server_address);
    if(bind(this->welcome_socket, adress, adress_len) < 0){
        perror("Error: bind");
        exit(1);
    }
}

void TCPConnection::listen_tcp(){
    int max_waiting_connections = 1;
    if (listen(this->welcome_socket, max_waiting_connections) < 0)
    {
        perror("ERROR: listen");
        exit(EXIT_FAILURE); 
    }
    while(1){
        int communication_socket = accept(this->welcome_socket, this->comm_addr, &(this->comm_addr_size));
        std::cout << "Accepted connection" << std::endl;
        if (communication_socket < 0)
        {
            perror("ERROR: accept");
            exit(EXIT_FAILURE);
        }
        pthread_t thread;
        int *client_socket_ptr = new int;
        *client_socket_ptr = communication_socket;
        pthread_create(&thread, NULL, handle_connection, (void*)client_socket_ptr);

    }

}

void *handle_connection(void *arg){
    char buffer[BUFFER_SIZE];
    int *client_socket_ptr = (int*)arg;
    int client_socket = *client_socket_ptr;
    delete client_socket_ptr;
    int send_l;
    bzero(buffer,BUFFER_SIZE);
    std::cout << "Waiting for HELLO" << std::endl;
    send_l = recv(client_socket, buffer, BUFFER_SIZE-1, 0);
    if (send_l <= 0){
        perror("recvfrom:");
        close(client_socket);
        pthread_exit(NULL);
    }
    
    std::string message(buffer);
    if (message == "HELLO\n"){
        std::cout << "Received HELLO" << std::endl;
        send(client_socket, "HELLO\n", 6, 0);
    }
    else{
        std::cout << "Invalid message" << std::endl;
        send(client_socket, "BYE\n",4, 0);
        close(client_socket);
        pthread_exit(NULL);
    }
    bzero(buffer,BUFFER_SIZE);
    while (1){
        send_l = recv(client_socket, buffer, BUFFER_SIZE, 0);
        std::cout << "Received message: " << buffer;
        if (send_l <= 0){
            perror("ERROR: recvfrom:");
            close(client_socket);
            pthread_exit(NULL);
        }
        std::string message(buffer);
        if (message == "BYE\n"){
            send(client_socket, "BYE\n", 4, 0);
            close(client_socket);
            pthread_exit(NULL);
        }
        size_t pos = message.find("SOLVE ");
        if (pos == std::string::npos or pos != 0){
            std::cout << "Invalid message" << std::endl;
            send(client_socket, "BYE\n", 4, 0);
            close(client_socket);
            pthread_exit(NULL);
        }
        message.erase(0,6);
        message.pop_back();
        Parser parser(message);
        try{
            parser.parse();
        }
        catch (...){
            std::cout << "Invalid message" << std::endl;
            send(client_socket, "BYE\n",4, 0);
            close(client_socket);
            pthread_exit(NULL);
        }
        message = "RESULT " + std::to_string(parser.result) + "\n";
        if (send(client_socket, message.c_str(), message.length(), 0) < 0){
            perror("ERROR: sendto:");
            close(client_socket);
            pthread_exit(NULL);
        }
        bzero(buffer,BUFFER_SIZE);
    }
    close(client_socket);
    pthread_exit(NULL);
}


