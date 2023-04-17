/*tokeniser.h
* auhor: Jakub Kontrik
* login: xkontr02
*/
#ifndef TOKENISER_H
#define TOKENISER_H
#include <optional>
#include <string>
// class with types of tokens
class Token{
    public:
    std::optional<int>number;
    std::optional<char>opera;
    std::optional<char>left_brace;
    std::optional<char>right_brace;
    std::optional<char>space;
};
// class for tokenising input string
class Tokeniser{
    public:
    Tokeniser(std::string input);
    Token get_token(int &i);
    private:
    std::string input;
    int load_number(int &i);
};

#endif // TOKENISER_H
