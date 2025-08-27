#include "lexer.h"

std::vector<Token> Lexer::tokenize(const std::string& source) {
    std::vector<Token> tokens;
    // Simple stub: treat entire source as identifier
    tokens.push_back({"IDENTIFIER", source});
    return tokens;
}
