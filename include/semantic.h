#pragma once
#include "ast.h"
#include <unordered_map>
#include <string>

struct Symbol {
    std::string name;
    std::string type;
};

class SemanticAnalyzer {
public:
    void analyze(ASTNode* root);

private:
    std::unordered_map<std::string, Symbol> symbolTable;
    void declareVariable(const std::string& name, const std::string& type);
    bool isDeclared(const std::string& name);
    std::string getType(const std::string& name);
    void checkAssignment(const std::string& name, const std::string& type);
    void checkExpression(ASTNode* node);
};
