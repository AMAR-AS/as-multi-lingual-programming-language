#ifndef SEMANTIC_H
#define SEMANTIC_H

#include <string>
#include <unordered_map>
#include "ast.h"

// -------------------------------------
// Symbol Information
// -------------------------------------
struct Symbol {
    std::string name;
    std::string type;
};

// -------------------------------------
// Semantic Analyzer
// -------------------------------------
class SemanticAnalyzer {
private:
    std::unordered_map<std::string, Symbol> symbolTable;

public:
    void analyze(ASTNode* root);
    void declareVariable(const std::string& name, const std::string& type);
    bool isDeclared(const std::string& name);
    std::string getType(const std::string& name);

private:
    void checkAssignment(const std::string& name, const std::string& type);
    void checkExpression(ASTNode* node);
};

#endif // SEMANTIC_H
