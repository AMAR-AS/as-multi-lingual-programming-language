#include "semantic.h"
#include <iostream>

// -------------------------------------------------
// Analyze AST Recursively
// -------------------------------------------------
void SemanticAnalyzer::analyze(ASTNode* root) {
    if (!root) return;

    if (root->nodeType == "Assignment") {
        if (root->children.size() < 2) {
            std::cerr << "[Semantic Error] Malformed assignment node.\n";
            return;
        }

        std::string var = root->children[0]->value;
        std::string exprType;

        // Check if variable is declared
        if (!isDeclared(var)) {
            std::cerr << "[Semantic Error] Variable '" << var << "' not declared!\n";
            return;
        }

        // Analyze RHS expression
        checkExpression(root->children[1]);
        exprType = root->children[1]->nodeType;

        // Validate assignment type
        checkAssignment(var, exprType);
    }

    // Recurse into children
    for (auto* child : root->children) {
        analyze(child);
    }
}

// -------------------------------------------------
// Declare a new variable
// -------------------------------------------------
void SemanticAnalyzer::declareVariable(const std::string& name, const std::string& type) {
    symbolTable[name] = {name, type};
}

// -------------------------------------------------
// Check if variable is declared
// -------------------------------------------------
bool SemanticAnalyzer::isDeclared(const std::string& name) {
    return symbolTable.find(name) != symbolTable.end();
}

// -------------------------------------------------
// Get type of variable
// -------------------------------------------------
std::string SemanticAnalyzer::getType(const std::string& name) {
    if (isDeclared(name))
        return symbolTable[name].type;
    return "undefined";
}

// -------------------------------------------------
// Validate assignment type
// -------------------------------------------------
void SemanticAnalyzer::checkAssignment(const std::string& name, const std::string& type) {
    std::string varType = getType(name);
    if (varType != type && type != "undefined") {
        std::cerr << "[Semantic Error] Type mismatch: cannot assign " 
                  << type << " to " << varType 
                  << " variable '" << name << "'\n";
    }
}

// -------------------------------------------------
// Check expression type
// -------------------------------------------------
void SemanticAnalyzer::checkExpression(ASTNode* node) {
    if (!node) return;

    if (node->nodeType == "Identifier") {
        if (!isDeclared(node->value)) {
            std::cerr << "[Semantic Error] Variable '" 
                      << node->value << "' not declared!\n";
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
