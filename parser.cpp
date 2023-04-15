#include "parser.h"
#include <iostream>
#include <string>
#include "tokeniser.h"
#include <stack>
Parser::Parser(std::string input): tokeniser(input){
    this->input = input;

}
void Parser::parse(){
    Token token = this->tokeniser.get_token(idx);
    std::stack<Token> stack;
    if (!token.left_brace.has_value()) {
        printf("SYNTAX ERROR");
        return;
    }
    stack.push(token);
    token = this->tokeniser.get_token(idx);
    if (!token.opera.has_value()) {
        printf("SYNTAX ERROR");
        return;
    }
    stack.push(token);
    token = this->tokeniser.get_token(idx);
    if (!token.space.has_value()) {
        printf("SYNTAX ERROR");
        return;
    }
    token = this->tokeniser.get_token(idx);
    if (!expr(token,stack)) {
        printf("SYNTAX ERROR");
        return;
    }
    token = this->tokeniser.get_token(idx);
    if (!token.space.has_value()) {
        printf("SYNTAX ERROR");
        return;
    }
    token = this->tokeniser.get_token(idx);
    if (!expr(token,stack)) {
        printf("SYNTAX ERROR");
        return;
    }
    token = this->tokeniser.get_token(idx);
    if (!end(token,stack)) {
        printf("SYNTAX ERROR");
        return;
    }
    if (!token.right_brace.has_value()) {
        printf("SYNTAX ERROR");
        return;
    }
    this->execute(stack);
    if (idx != (int)input.length()) {
        printf("SYNTAX ERROR");
        return;
    }
    std::cout << "Result: " << this->result << std::endl;
}

bool Parser::expr(Token &token, std::stack<Token> &stack){
    bool ret = false;
    if (token.left_brace.has_value()){
        stack.push(token);
        token = this->tokeniser.get_token(idx);
        if(!token.opera.has_value()){
            ret = false;
        }
        stack.push(token);
        token = this->tokeniser.get_token(idx);
        if (!token.space.has_value()){
            ret = false;
        }
        token = this->tokeniser.get_token(idx);
        if (!expr(token,stack)){
            ret = false;
        }
        token = this->tokeniser.get_token(idx);
        if (!token.space.has_value()){
            ret = false;
        }
        token = this->tokeniser.get_token(idx);
        if(!expr(token,stack)){
            ret = false;
        }
        token = this->tokeniser.get_token(idx);
        if (!end(token,stack)){
            ret = false;
        }
        if (token.right_brace.has_value()){
            ret = true;
        }
        this->execute(stack);
    }
    else if (token.number.has_value()){
        ret = true;
        stack.push(token);
    }
    return ret;
}
bool Parser::end(Token &token, std::stack<Token> &stack){
    bool ret = false;
    if (token.space.has_value()){
        token=this->tokeniser.get_token(idx);
        if (expr(token,stack)){
            token = this->tokeniser.get_token(idx);
            ret = end(token,stack);
        }
    }
    else{
        ret = true;
    }
    return ret;
}

void Parser::execute(std::stack<Token> &stack){
    Token token = stack.top();
    stack.pop();
    std::stack<int> stack2;
    char operation=' ';
    while (1){
        if (token.number.has_value()){
            stack2.push(token.number.value());
        }
        else if (token.opera.has_value()){
            if (token.opera.value() == '+'){ 
                operation = '+';
                break;
            }
            else if (token.opera.value() == '-'){
                operation = '-';
                break;
            }
            else if (token.opera.value() == '*'){ 
                operation = '*';
                break;
            }
            else if (token.opera.value() == '/'){ 
                operation = '/';
                break;
            }
        }
        else{
            break;
        }
        token = stack.top();
        stack.pop();
    }
    stack.pop();
    int size;
    size = stack2.size();
    this->result = stack2.top();
    stack2.pop();
    switch (operation){
        case '+':
            for (int i = 0; i < size-1; i++) {
                result += stack2.top();
                stack2.pop();
            }
            break;
        case '-':
            for (int i = 0; i < size-1; i++) {
                result -= stack2.top();
                stack2.pop();
            }
            break;
        case '*':
            for (int i = 0; i < size-1; i++) {
                result *= stack2.top();
                stack2.pop();
            }
            break;
        case '/':
            for (int i = 0; i < size-1; i++) {
                result /= stack2.top();
                stack2.pop();
            }
            break;
    }
    Token restul;
    restul.number = result;
    stack.push(restul);
}