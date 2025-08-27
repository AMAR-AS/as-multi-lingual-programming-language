#include "semantic.h"
#include <iostream>

// --------------------------------------------
// Analyze the AST recursively
// --------------------------------------------
void SemanticAnalyzer::analyze(ASTNode* root) {
    if (!root) return;

    if (root->nodeType == "Assignment") {
        if (root->children.size() < 2) {
            std::cerr << "[Semantic Error] Malformed assignment node.\n";
            return;
        }

        std::string var = root->children[0]->value;

        // Check if variable is declared
        if (!isDeclared(var)) {
            std::cerr << "[Semantic Error] Variable '" << var << "' not declared!\n";
            return;
        }

        // Analyze expression (RHS)
        checkExpression(root->children[1]);
        std::string exprType = root->children[1]->nodeType;

        // Validate assignment type
        checkAssignment(var, exprType);
    }

    // Recursively analyze all children
    for (auto* child : root->children) {
        analyze(child);
    }
}

// --------------------------------------------
// Variable Declaration
// --------------------------------------------
void SemanticAnalyzer::declareVariable(const std::string& name, const std::string& type) {
    if (isDeclared(name)) {
        std::cerr << "[Semantic Error] Variable '" << name << "' already declared.\n";
        return;
    }
    symbolTable[name] = {name, type};
}

// --------------------------------------------
// Symbol Lookup
// --------------------------------------------
bool SemanticAnalyzer::isDeclared(const std::string& name) {
    return symbolTable.find(name) != symbolTable.end();
}

std::string SemanticAnalyzer::getType(const std::string& name) {
    auto it = symbolTable.find(name);
    if (it != symbolTable.end())
        return it->second.type;
    return "undefined";
}

// --------------------------------------------
// Assignment Type Checking
// --------------------------------------------
void SemanticAnalyzer::checkAssignment(const std::string& name, const std::string& exprType) {
    std::string varType = getType(name);
    if (varType != exprType && exprType != "undefined") {
        std::cerr << "[Semantic Error] Type mismatch: cannot assign type '" << exprType
                  << "' to variable '" << name << "' of type '" << varType << "'\n";
    }
}

// --------------------------------------------
// Expression Type Inference
// --------------------------------------------
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
    else {
        // Recursively infer for children (expressions, binary ops, etc.)
        for (auto* child : node->children) {
            checkExpression(child);
        }
    }
}
