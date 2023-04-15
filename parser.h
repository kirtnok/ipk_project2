#ifndef PARSER_H
#define PARSER_H
#include "tokeniser.h"
#include <string>
class Parser{
    public:
    std::string input;
    Tokeniser tokeniser;
    int result;
    int idx=0;
    Parser(std::string input);
    void parse();
    bool expr(Token &token);
    bool end(Token &token);
};

#endif // PARSER_H
