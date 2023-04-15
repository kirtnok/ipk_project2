#include <iostream>
#include <math.h>
#include <string>
#include <stack>
#include <optional>

#include "tokeniser.h"
#include "parser.h"




int main() {
    std::string input_string;
    std::getline(std::cin, input_string);
    Parser parser(input_string);
    parser.parse();
 
    return 0;
}
