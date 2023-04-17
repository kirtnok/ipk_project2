/*ipkcpd.cpp
* auhor: Jakub Kontrik
* login: xkontr02
* brief: server for ipk calculator
*/
#include <iostream>
#include <math.h>
#include <string>
#include <stack>
#include <optional>
#include <string.h>
#include <unistd.h>

#include "tcpconnection.h"
#include "tokeniser.h"
#include "parser.h"
#include "udpconnection.h"
#include <signal.h>
bool sigint_flag = false;

void sigint_handle(int signum){
    (void)signum;
    sigint_flag = true;
}

void print_help(){
    std::cout << "Usage: ipkcpd -h <host> -p <port> -m <mode> [--help]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "-h <host>   IPv4 address the server will listen on" << std::endl;
    std::cout << "-p <port>   Port number the server will listen on" << std::endl;
    std::cout << "-m <mode>   Mode, either tcp or udp" << std::endl;
    std::cout << "--help          Print this help" << std::endl;
}


int main(int argc, char *argv[]){
    int arg, port;
    const char *host;
    char *p;
    bool tcp_mode = false;
    bool udp_mode = false;
    //set up signal handler
    struct sigaction sigint_handler;
    sigint_handler.sa_handler = sigint_handle;
    sigemptyset(&sigint_handler.sa_mask);
    sigint_handler.sa_flags = 0;
    sigaction(SIGINT, &sigint_handler, NULL);
    if(argc == 2 && !strcmp(argv[1], "--help")){
        print_help();
        exit(0);
    }
    if (argc != 7){
        std::cerr << "Wrong number of arguments" << std::endl;
        std::cout << "Use --help for help" << std::endl;
        exit(1);
    }
    // parsing arguments
    while ((arg = getopt(argc, argv, "h:p:m:"))!= -1){
        switch (arg){
            case 'h':
                host = optarg;
                break;
            case 'p':
                port = strtol(optarg,&p,10);
                //checking if port is in range and if it is a number
                if (*p != '\0' || port < 0 || port > 65535){
                    std::cerr << "Error: wrong port format" << std::endl;
                    exit(1);
                }
                break;
            case 'm':
                if (!strcmp(optarg,"udp")){
                    udp_mode = true;
                }
                else if(!strcmp(optarg,"tcp")){
                    tcp_mode = true;
                }
                else{
                    std::cerr << "Error: wrong mode" << std::endl;
                    std::cout << "Use --help for help" << std::endl;
                    exit(1);
                }
                break;
            case '?':
                std::cerr << "Error: unknown option: " << (char)optopt << std::endl;
                std::cout << "Use --help for help" << std::endl;
                exit(1);
        }
    }
    if (udp_mode == true){
        UDPConnection connection(host,port);
        std::cout << "Listening" << std::endl;
        connection.listen();
        close(connection.server_socket);
    }
    else if (tcp_mode == true){
        TCPConnection connection(host,port);
        std::cout << "Listening" << std::endl;
        connection.listen_tcp();
        close(connection.welcome_socket);
    }
 
    return 0;
}
