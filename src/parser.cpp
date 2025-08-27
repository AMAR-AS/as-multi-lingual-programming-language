#include "parser.h"

ASTNode* Parser::parse(const std::vector<Token>& tokens) {
    if (tokens.empty()) return nullptr;
    ASTNode* root = new ASTNode("Program");
    for (auto& tok : tokens) {
        root->children.push_back(new ASTNode("Identifier", tok.value));
    }
    return root;
}
if (match(TokenType::PRINT)) {
    Expr* value = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after value.");
    return new PrintStmt(value);
}
