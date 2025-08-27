#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "parser.h"
#include <string>
#include <unordered_map>
#include <vector>

struct Symbol {
    std::string name;
    std::string type;  // "num", "txt", "dec", etc.
};

class SemanticAnalyzer {
private:
    std::unordered_map<std::string, Symbol> symbolTable;

public:
    void analyze(ASTNode* root);
    void declareVariable(const std::string& name, const std::string& type);
    bool isDeclared(const std::string& name);
    std::string getType(const std::string& name);
    void checkAssignment(const std::string& name, const std::string& type);
    void checkExpression(ASTNode* node);
};

#endif
