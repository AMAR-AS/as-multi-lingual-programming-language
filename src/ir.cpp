#include "ir.h"
#include <iostream>

// -------------------------------------------------
// Recursive IR Generation
// -------------------------------------------------
void IRGenerator::generate(ASTNode* root) {
    if (!root) return;

    if (root->nodeType == "Assignment") {
        if (root->children.size() < 2) {
            std::cerr << "[IR Error] Malformed assignment node.\n";
            return;
        }

        std::string var = root->children[0]->value;
        std::string expr = root->children[1]->value;

        IRInstruction instr = {"MOV", expr, "", var};
        instructions.push_back(instr);
    }
    else if (root->nodeType == "BinaryOp") {
        if (root->children.size() < 2) {
            std::cerr << "[IR Error] Malformed binary operation node.\n";
            return;
        }

        std::string left = root->children[0]->value;
        std::string right = root->children[1]->value;

        // Temporary variable for the result
        std::string result = "t" + std::to_string(tempCounter++);

        IRInstruction instr = {root->value, left, right, result};
        instructions.push_back(instr);

        // Replace this node’s value with temp var so parents can use it
        root->value = result;
    }
    else if (root->nodeType == "Number" || root->nodeType == "String" || root->nodeType == "Identifier") {
        // Literals and identifiers don’t directly produce instructions
        // but they are operands for others
    }

    // Recursively generate IR for children
    for (auto* child : root->children) {
        generate(child);
    }
}

// -------------------------------------------------
// Print IR instructions
// -------------------------------------------------
void IRGenerator::printIR() {
    for (auto& instr : instructions) {
        std::cout << instr.op << " "
                  << instr.arg1 << " "
                  << instr.arg2 << " -> "
                  << instr.result << "\n";
    }
}

// -------------------------------------------------
// Return IR instructions
// -------------------------------------------------
std::vector<IRInstruction> IRGenerator::getIR() {
    return instructions;
}
