#ifndef TOKENISER_H
#define TOKENISER_H
#include <optional>
#include <string>

class Token{
    public:
    std::optional<int>number;
    std::optional<char>opera;
    std::optional<char>left_brace;
    std::optional<char>right_brace;
    std::optional<char>space;
};

class Tokeniser{
    public:
    Tokeniser(std::string input);
    Token get_token(int &i);
    private:
    std::string input;
    int load_number(int &i);

};

#endif // TOKENISER_H
