/*parser.h
* auhor: Jakub Kontrik
* login: xkontr02
*/
#ifndef PARSER_H
#define PARSER_H
#include "tokeniser.h"
#include <string>
#include <stack>
// class for parsing queries and executing them
class Parser{
    public:
    std::string input;
    Tokeniser tokeniser;
    int result;
    int idx=0;
    Parser(std::string input);
    void parse();
    bool expr(Token &token,std::stack<Token> &stack);
    bool end(Token &token,std::stack<Token> &stack);
    void execute(std::stack<Token> &stack);
};

#endif // PARSER_H
