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
#include <mutex>

std::mutex mtx;

#define BUFFER_SIZE 1025
struct thread_args {
    int client_socket;
    std::vector<int>* client_sockets_ptr;
};
struct thread_args *args;

TCPConnection::TCPConnection(const char* host, int port){
    this->comm_addr={};
    this->comm_addr_size = sizeof(this->comm_addr);
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
    std::vector<pthread_t> threads;
    while(1){
        pthread_t thread;
        int communication_socket = accept(this->welcome_socket, this->comm_addr, &(this->comm_addr_size));
        if (communication_socket < 0 and !sigint_flag)
        {
            perror("ERROR: accept");
            exit(EXIT_FAILURE);
        }
        if (sigint_flag){
            std::cout << "Closing server" << std::endl;
            for (int i = 0; i < this->client_sockets.size(); i++){
                send(this->client_sockets[i], "BYE\n", 4, 0);
                shutdown(this->client_sockets[i], SHUT_RDWR);
                close(this->client_sockets[i]);

            }
            break;
        }
        this->client_sockets.push_back(communication_socket);
        std::cout << "Accepted connection" << std::endl;
        args = new thread_args;
        args->client_socket = communication_socket;
        args->client_sockets_ptr = &this->client_sockets;
        pthread_create(&thread, NULL, handle_connection, (void*)args);
        threads.push_back(thread);
    }
    std::cout << "Closing server" << std::endl;
    for (auto &thread : threads) {
        pthread_join(thread, NULL);
    }

}

void *handle_connection(void *arg){
    char buffer[BUFFER_SIZE];
    thread_args *args = (thread_args*)arg;
    int client_socket = args->client_socket;
    std::vector<int>* client_sockets_ptr = args->client_sockets_ptr;
    
    delete args;
    int send_l;
    bzero(buffer,BUFFER_SIZE);
    std::cout << "Waiting for HELLO" << std::endl;
    send_l = recv(client_socket, buffer, BUFFER_SIZE-1, 0);
    if (send_l <= 0){
        perror("recvfrom:");
        mtx.lock();
        for (auto socket = client_sockets_ptr->begin(); socket != client_sockets_ptr->end(); socket++) {
            if (*socket == client_socket) {
                client_sockets_ptr->erase(socket);
                break;
            }
        }
        mtx.unlock();
        std::cout << client_socket << std::endl;
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
        mtx.lock();
        for (auto socket = client_sockets_ptr->begin(); socket != client_sockets_ptr->end(); socket++) {
            if (*socket == client_socket) {
                client_sockets_ptr->erase(socket);
                break;
            }
        }
        mtx.unlock();
        send(client_socket, "BYE\n",4, 0);
        close(client_socket);
        pthread_exit(NULL);
    }
    bzero(buffer,BUFFER_SIZE);
    while (1){
        send_l = recv(client_socket, buffer, BUFFER_SIZE, 0);
        std::cout << "Received message: " << buffer;
        if (send_l <= 0){
            perror("recvfrom:");
            mtx.lock();
            for (auto socket = client_sockets_ptr->begin(); socket != client_sockets_ptr->end(); socket++) {
                if (*socket == client_socket) {
                    client_sockets_ptr->erase(socket);
                    break;
                }
            }
            mtx.unlock();
            close(client_socket);
            pthread_exit(NULL);
        }
        std::string message(buffer);
        if (message == "BYE\n"){
            break;
        }
        size_t pos = message.find("SOLVE ");
        if (pos == std::string::npos or pos != 0){
            std::cout << "Invalid message" << std::endl;
            break;
        }
        message.erase(0,6);
        message.pop_back();
        Parser parser(message);
        try{
            parser.parse();
        }
        catch (...){
            std::cout << "Invalid message" << std::endl;
            break;
        }
        message = "RESULT " + std::to_string(parser.result) + "\n";
        if (send(client_socket, message.c_str(), message.length(), 0) < 0){
            perror("ERROR: sendto:");
            mtx.lock();
            for (auto socket = client_sockets_ptr->begin(); socket != client_sockets_ptr->end(); socket++) {
                if (*socket == client_socket) {
                    client_sockets_ptr->erase(socket);
                    break;
                }
            }
            mtx.unlock();
            close(client_socket);
            pthread_exit(NULL);
        }
        bzero(buffer,BUFFER_SIZE);
    }
    
    mtx.lock();
    for (auto socket = client_sockets_ptr->begin(); socket != client_sockets_ptr->end(); socket++) {
        if (*socket == client_socket) {
            client_sockets_ptr->erase(socket);
            break;
        }
    }
    
    // unlock the mutex after modifying the vector
    mtx.unlock();
    send(client_socket, "BYE\n", 4, 0);
    close(client_socket);
    pthread_exit(NULL);
}


