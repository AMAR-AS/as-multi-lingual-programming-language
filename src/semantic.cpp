#include "semantic.h"
#include <iostream>

void SemanticAnalyzer::analyze(ASTNode* root) {
    if (!root) return;
    for (auto child : root->children) {
        analyze(child);
    }
}

void SemanticAnalyzer::declareVariable(const std::string& name, const std::string& type) {
    symbolTable[name] = {name, type};
}

bool SemanticAnalyzer::isDeclared(const std::string& name) {
    return symbolTable.find(name) != symbolTable.end();
}

std::string SemanticAnalyzer::getType(const std::string& name) {
    if (isDeclared(name)) return symbolTable[name].type;
    return "undefined";
}

void SemanticAnalyzer::checkAssignment(const std::string& name, const std::string& type) {
    std::string varType = getType(name);
    if (varType != type) {
        std::cerr << "[Semantic Error] Type mismatch for " << name << std::endl;
    }
}

void SemanticAnalyzer::checkExpression(ASTNode* node) {
    if (!node) return;
}
