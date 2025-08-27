#include "semantic.h"
#include <iostream>

void SemanticAnalyzer::analyze(ASTNode* root) {
    if (!root) return;

    if (root->nodeType == "Assignment") {
        std::string var = root->children[0]->value;
        std::string exprType;

        // Check if variable is declared
        if (!isDeclared(var)) {
            std::cerr << "[Semantic Error] Variable '" << var << "' not declared!\n";
            return;
        }

        // Check expression type
        checkExpression(root->children[1]);
        exprType = root->children[1]->nodeType;

        // Validate assignment type
        checkAssignment(var, exprType);
    }

    // Recursively analyze children
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
    if (isDeclared(name))
        return symbolTable[name].type;
    return "undefined";
}

void SemanticAnalyzer::checkAssignment(const std::string& name, const std::string& type) {
    std::string varType = getType(name);
    if (varType != type) {
        std::cerr << "[Semantic Error] Type mismatch: cannot assign " << type
                  << " to " << varType << " variable '" << name << "'\n";
    }
}

void SemanticAnalyzer::checkExpression(ASTNode* node) {
    if (!node) return;

    if (node->nodeType == "Identifier") {
        if (!isDeclared(node->value)) {
            std::cerr << "[Semantic Error] Variable '" << node->value << "' not declared!\n";
            node->nodeType = "undefined";
        } else {
            node->nodeType = getType(node->value);
        }
    }
    else if (node->nodeType == "Number") {
        node->nodeType = "num";
    }
    else if (node->nodeType == "String") {
        node->nodeType = "txt";
    }
}
