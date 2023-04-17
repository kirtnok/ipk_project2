/*tcpconnection.cpp
* auhor: Jakub Kontrik
* login: xkontr02
* brief: tcp connection implementation
*/
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
#include <mutex>

#include "tcpconnection.h"
#include "parser.h"
// mutex for ereasing client socekets from vector
std::mutex mtx;

// struct for passing arguments to thread
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
        exit(EXIT_FAILURE);
    }
    // after closing the socket, is ready to be reused
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
        exit(EXIT_FAILURE);
    }
}

// fuction for handling client requests
void TCPConnection::listen_tcp(){
    int max_waiting_connections = 2;
    if (listen(this->welcome_socket, max_waiting_connections) < 0)
    {
        perror("ERROR: listen");
        exit(EXIT_FAILURE); 
    }
    // vector for storing threads
    std::vector<pthread_t> threads;
    // waiting for clients
    while(1){
        // creating thread for each client
        pthread_t thread;
        int communication_socket = accept(this->welcome_socket, this->comm_addr, &(this->comm_addr_size));
        if (communication_socket < 0 and !sigint_flag)
        {
            perror("ERROR: accept");
            exit(EXIT_FAILURE);
        }
        // if SIGINT is caught,send to all clients BYE and close all connections
        if (sigint_flag){
            std::cout << "Closing server" << std::endl;
            for (int i = 0; i < (int)this->client_sockets.size(); i++){
                send(this->client_sockets[i], "BYE\n", 4, 0);
                shutdown(this->client_sockets[i], SHUT_RDWR);
                close(this->client_sockets[i]);

            }
            break;
        }
        this->client_sockets.push_back(communication_socket);
        std::cout << "Accepted connection" << std::endl;
        // passing arguments to thread
        args = new thread_args;
        args->client_socket = communication_socket;
        args->client_sockets_ptr = &this->client_sockets;
        pthread_create(&thread, NULL, handle_connection, (void*)args);
        threads.push_back(thread);
    }
    // waiting for all threads to finish
    for (auto &thread : threads) {
        pthread_join(thread, NULL);
    }

}
// thread function to handle each client
void *handle_connection(void *arg){
    // getting arguments
    thread_args *args = (thread_args*)arg;
    int client_socket = args->client_socket;
    std::vector<int>* client_sockets_ptr = args->client_sockets_ptr;
    delete args;

    char buffer[BUFFER_SIZE];
    bzero(buffer,BUFFER_SIZE);
    std::string line;
    std::string full_message;
    bool hello_flag = false;
    bool exiting = false;

    std::cout << "Waiting for HELLO" << std::endl;
    while(!exiting){
        if (recv(client_socket, buffer, BUFFER_SIZE-1, 0) <= 0){
            perror("recvfrom:");
            // preventing race condition
            mtx.lock();
            // removing client socket from vector
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
        std::string buffer_s(buffer);
        // adding loaded buffer to full message
        full_message = full_message + buffer_s;
        bzero(buffer,BUFFER_SIZE);
        // loading buffer until new line is found
        if (buffer_s.find('\n') == std::string::npos){
            continue;
        }
        // parsing message
        while (!full_message.empty() and full_message.find('\n') != std::string::npos){
            // getting first line
            line = full_message.substr(0, full_message.find('\n'));
            // removing first line from full message
            full_message = full_message.substr(full_message.find('\n')+1);
            std::cout <<"Message "<< line << std::endl;
            if (line == "HELLO"){
                if (hello_flag == true){
                    exiting = true;
                    break;
                }
                hello_flag = true;
                if (send(client_socket, "HELLO\n", 6, 0)< 0){
                    perror("ERROR: sendto:");
                    mtx.lock();
                    for (auto socket = client_sockets_ptr->begin(); socket != client_sockets_ptr->end(); socket++){
                        if (*socket == client_socket) {
                            client_sockets_ptr->erase(socket);
                            break;
                        }
                    }
                    mtx.unlock();
                    close(client_socket);
                    pthread_exit(NULL);
                }

            }
            else if(hello_flag == true){
                if (line == "BYE"){
                    exiting = true;
                    break;
                }
                // checking solve message
                else{
                    size_t pos = line.find("SOLVE ");
                    if (pos == std::string::npos or pos != 0){
                        std::cout << "Invalid message" << std::endl;
                        exiting = true;
                        break;
                    }
                    line.erase(0,6);
                    Parser parser(line);
                    try{
                        parser.parse();
                    }
                    // catching any exception from parser
                    catch (...){
                        std::cout << "Invalid message" << std::endl;
                        exiting = true;
                        break;
                    }
                    line = "RESULT " + std::to_string(parser.result) + "\n";
                    if (send(client_socket, line.c_str(), line.length(), 0) < 0){
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
            }  
            else{
                std::cout << "Invalid message" << std::endl;
                exiting = true;
                break;
            }
        }
        line.clear();
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
