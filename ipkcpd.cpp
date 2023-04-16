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
        UDPConnection connection((char*)host,port);
        std::cout << "Listening" << std::endl;
        connection.listen();
    }
    else if (tcp_mode == true){
        TCPConnection connection(host,port);
        std::cout << "Listening" << std::endl;
        connection.listen_tcp();
    }
    std::string input_string;
    std::getline(std::cin, input_string);
    Parser parser(input_string);
    parser.parse();
 
    return 0;
}
