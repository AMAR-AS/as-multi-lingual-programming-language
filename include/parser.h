#pragma once
#include "ast.h"
#include "lexer.h"

class Parser {
public:
    ASTNode* parse(const std::vector<Token>& tokens);
};
