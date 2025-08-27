#pragma once
#include <string>
#include <vector>

struct Token {
    std::string type;
    std::string value;
};

class Lexer {
public:
    std::vector<Token> tokenize(const std::string& source);
};
