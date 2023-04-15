#include "tokeniser.h"
#include <string>
#include <iostream>

Tokeniser::Tokeniser(std::string input){
    this->input = input;
}

Token Tokeniser::get_token(int &i){
    Token token;
    if (std::isspace(input[i])) {
        token.space = 'y';
    }
    else if (input[i] == '('){
        token.left_brace = '(';
    }
    else if (input[i]=='+' or input[i] == '-' or input[i] == '*' or input[i]== '/'){
        token.opera = input[i];
    }
    else if (std::isdigit(input[i])){
        token.number = load_number(i);
        return token;
    }
    else if (input[i] == ')'){
        token.right_brace= ')';

    }
    else{
        std::cout << "Error" << std::endl;
        throw 1;
    }
    i++;
    return token;
};
int Tokeniser::load_number(int &i){
        int number = 0;
        while (std::isdigit(input[i])){
            number = number*10 + std::stoi(input.substr(i,1));
            i++;
        }
        return number;
}
