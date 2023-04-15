#include "parser.h"
#include <iostream>
#include <string>
#include "tokeniser.h"
Parser::Parser(std::string input): tokeniser(input){
    this->input = input;

}
void Parser::parse(){
    Token token = this->tokeniser.get_token(idx);
    if (!token.left_brace.has_value()) {
        printf("SYNTAX ERROR");
        return;
    }
    token = this->tokeniser.get_token(idx);
    if (!token.opera.has_value()) {
        printf("SYNTAX ERROR");
        return;
    }
    token = this->tokeniser.get_token(idx);
    if (!token.space.has_value()) {
        printf("SYNTAX ERROR");
        return;
    }
    token = this->tokeniser.get_token(idx);
    if (!expr(token)) {
        printf("SYNTAX ERROR");
        return;
    }
    token = this->tokeniser.get_token(idx);
    if (!token.space.has_value()) {
        printf("SYNTAX ERROR");
        return;
    }
    token = this->tokeniser.get_token(idx);
    if (!expr(token)) {
        printf("SYNTAX ERROR");
        return;
    }
    token = this->tokeniser.get_token(idx);
    if (!end(token)) {
        printf("SYNTAX ERROR");
        return;
    }
    if (!token.right_brace.has_value()) {
        printf("SYNTAX ERROR");
        return;
    }
    if (idx != (int)input.length()) {
        printf("SYNTAX ERROR");
        return;
    }
    printf("SYNTAX OK");
}

bool Parser::expr(Token &token){
    bool ret = false;
    if (token.left_brace.has_value()){
        token = this->tokeniser.get_token(idx);
        if(!token.opera.has_value()){
            ret = false;
        }
        token = this->tokeniser.get_token(idx);
        if (!token.space.has_value()){
            ret = false;
        }
        token = this->tokeniser.get_token(idx);
        if (!expr(token)){
            ret = false;
        }
        token = this->tokeniser.get_token(idx);
        if (!token.space.has_value()){
            ret = false;
        }
        token = this->tokeniser.get_token(idx);
        if(!expr(token)){
            ret = false;
        }
        token = this->tokeniser.get_token(idx);
        if (!end(token)){
            ret = false;
        }
        if (token.right_brace.has_value()){
            ret = true;
        }
    }
    else if (token.number.has_value()){
        ret = true;
    }
    return ret;
}
bool Parser::end(Token &token){
    bool ret = false;
    if (token.space.has_value()){
        token=this->tokeniser.get_token(idx);
        if (expr(token)){
            token = this->tokeniser.get_token(idx);
            ret = end(token);
        }
    }
    else{
        ret = true;
    }
    return ret;
}

