# ipk_projekt2

Implementation of server aplication of the IPK Calculator
in the C++. Program is limited only for linux operating. Server is loging some messages to stdout.


## Usage
ipkcpd -h \<host\> -p \<port> -m \<mode> [--help]

-h \<host>   IPv4 address the server will listen on  
-p \<port>   Port number the server will listen on
-m \<mode>   Mode, either tcp or udp  

## Code structure
The code is divided into several files. 
- ipkcpd.cpp - main function, argument parsing and creating connection object
- parser.cpp - implementation of parser that is parsing query and storing result
- tokeniser.cpp - implementation of tokeniser that is used by parser
- udpconnection.cpp - implementation of udp connection
- tcpconnection.cpp - implementation of tcp connection

### Parsing command line arguments
Arguments are parsed in `main()` after variable initializations, `getopt()` function is used in loop where are arguments also checked if they are in valid format.
### Body
After parsing arguments, the program creates connection object based on the mode. Then it calls `listen()` method of the connection object.

### UDP connection
UDP connection is implemented in `UDPConnection` class. In constructor there is socket created, socket option for instantly using closed socket set and socket bind.It has `listen()` method that is in loop waiting for client message. If sigint is recieved just loop is ended. After receiving message, it is checked if recieved first bytes are valid(defined by protocol), buffer is saved into string and sliced by recieved length. After that `Parser` object is created and message is validated. If message is valid, result is converted into string and sent back to client. With second byte set to 0. If message is invalid, error message is sent back to client with second byte set to 1.

### TCP connection
TCP connection is implemented in `TCPConnection` class. In constructor there is socket created, socket option for instantly using closed socket set and socket bind. It has `listen_tcp()` method that is in loop waiting for client connection. Maximum queue is set to 2 clients, but threads are used so it wont be needed anyway. After connection is accepted, new thread is created and for each client is `handle_client()` function called. Every client socket is saved into vector. `handle_client()` loads message from client till new line is recieved. Then message is checked, if error occurs or if client has sent `BYE\n` client socket is removed form list with help of mutex, `BYE\n` message is sent back to client and thread is ended. Solving handles again `Parser` object and if message is valid, result is converted into string and sent back to client with `RESULT` prefix. If sigint is recieved, to every client socket is sent `BYE\n` message and is closed. After end of loop there is waiting for every thread to end.

### Parser
Parser is implemented in `Parser` class. It has `parse()` method that is parsing input query and saving result. It is using `Tokeniser` object to tokenize input. Parser is using defined grammar for queries. It is using recursive descent parsing. If token is valid it is pushed into stack(not space) and next token is loaded. After right brace is loaded `execute()` method is called. Execute method is using stack to get tokens and execute them. Every element is popped out. If token is number, it is again pushed into second stack. If operator is loaded it is saved.
After finding operator loop is ended and numbers form second stack are evaluated. After evaluating result is pushed back onto stack.


## Testing
Testing has been done during the whole process of the implementation, every module has been tested. For communication with server, netcat or client was used.
For testing multiple clients at once, multiple terminal sessions were used.

### Parser
| Input | Output |-|
|-------|--------|----------|
| (+ 1 2) | SYNTAX OK R: 3 |passed|
| (+ 1 2 3) | SYNTAX OK R: 6 |passed|
| (/ 1 0) | ERROR |passed|
| (- 1 2) | ERROR |passed|
| (+ (* 1 2 3) (/ 1 2) 3 4) | SYNTAX OK R: 13 |passed|
| (+ 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0) | SYNTAX OK R: 1 |passed|
| (*+ 4 5) | SYNTAX ERROR |passed|
| (* 4  5) | SYNTAX ERROR |passed|

### UDP connection
| Input | Client output |-|
|-------|--------|---------|
| (+ 1 2) | OK:3 |passed|
| HELLO | ERROR:errormsg |passed|
| (+ 1 2 | ERROR:errormsg |passed|

### TCP connection
| Input | Client output |-|
|-------|--------|---------|
| HELLO\nSOLVE(+ 1 2)\nBYE\n| HELLO\nRESULT 3\nBYE\n|passed|
| HELLO\nSOLVE(+ 1 2)SOLVE\n| HELLO\n BYE|passed|
| SOLVE(+ 1 2)\n| BYE\n|passed|

## Bibliography
[C++ Documentation](https://learn.microsoft.com/en-us/cpp/cpp/?view=msvc-170)  
[STUBS](https://git.fit.vutbr.cz/NESFIT/IPK-Projekty/src/branch/master/Stubs)  
[Lectures](https://moodle.vut.cz/mod/folder/view.php?id=289124)
