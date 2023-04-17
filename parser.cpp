/*parser.cpp
* auhor: Jakub Kontrik
* login: xkontr02
* brief: parser for ipk calculator
*/
#include <iostream>
#include <string>
#include <stack>

#include "parser.h"
#include "tokeniser.h"

Parser::Parser(std::string input): tokeniser(input){
    this->input = input;
}

// query rule
void Parser::parse(){
    // load token
    Token token = this->tokeniser.get_token(idx);
    std::stack<Token> stack;
    std::cout << "Solving: " << input << std::endl;
    // implemented grammar rules if the token is not valid, throw -1
    if (!token.left_brace.has_value()) {
        std::cout << "Wrong query syntax" << std::endl;
        throw -1;
    }
    // push the token into the stack for evaluation
    stack.push(token);
    token = this->tokeniser.get_token(idx);
    if (!token.opera.has_value()) {
        std::cout << "Wrong query syntax" << std::endl;
        throw -1;
    }
    stack.push(token);
    token = this->tokeniser.get_token(idx);
    // if token is space, token is not pushed
    if (!token.space.has_value()) {
        std::cout << "Wrong query syntax" << std::endl;
        throw -1;
    }
    token = this->tokeniser.get_token(idx);
    if (!expr(token,stack)) {
        std::cout << "Wrong query syntax" << std::endl;
        throw -1;
    }
    token = this->tokeniser.get_token(idx);
    if (!token.space.has_value()) {
        std::cout << "Wrong query syntax" << std::endl;
        throw -1;
    }
    token = this->tokeniser.get_token(idx);
    if (!expr(token,stack)) {
        std::cout << "Wrong query syntax" << std::endl;
        throw -1;
    }
    token = this->tokeniser.get_token(idx);
    // looping rule for more than 2 expressions
    if (!end(token,stack)) {
        std::cout << "Wrong query syntax" << std::endl;
        throw -1;
    }
    if (!token.right_brace.has_value()) {
        std::cout << "Wrong query syntax" << std::endl;
        throw -1;
    }
    // after right brace, execute the stack
    this->execute(stack);
    if (idx != (int)input.length()) {
        std::cout << idx << input.length() << std::endl;
        std::cout << "Wrong query syntax" << std::endl;
        throw -1;
    }
    if (result < 0) {
        throw -1;
    }
    std::cout << "Result: " << this->result << std::endl;
}
// expression rule
bool Parser::expr(Token &token, std::stack<Token> &stack){
    bool ret = false;
    // expression rule has same structure as query rule
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
    // end rule for looping
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
// execute stack with pushed tokens
void Parser::execute(std::stack<Token> &stack){
    Token token = stack.top();
    stack.pop();
    // second stack for evaluation
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
    // evaluate numbers
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
                // if divided by 0, throw exception
                if (stack2.top() == 0) {
                    throw -1;
                }
                result /= stack2.top();
                stack2.pop();
            }
            break;
    }
    // push result into stack
    Token restul;
    restul.number = result;
    stack.push(restul);
}
