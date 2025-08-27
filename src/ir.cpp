#include "ir.h"
#include <iostream>

void IRGenerator::generate(ASTNode* root) {
    if (!root) return;

    if (root->nodeType == "Assignment") {
        std::string var = root->children[0]->value;
        std::string expr = root->children[1]->value;

        IRInstruction instr = {"MOV", expr, "", var};
        instructions.push_back(instr);
    }
    else if (root->nodeType == "BinaryOp") {
        std::string left = root->children[0]->value;
        std::string right = root->children[1]->value;
        std::string result = "t" + std::to_string(instructions.size());

        IRInstruction instr = {root->value, left, right, result};
        instructions.push_back(instr);
    }

    // Recursively generate IR for children
    for (auto child : root->children) {
        generate(child);
    }
}

void IRGenerator::printIR() {
    for (auto& instr : instructions) {
        std::cout << instr.op << " " 
                  << instr.arg1 << " " 
                  << instr.arg2 << " -> " 
                  << instr.result << std::endl;
    }
}

std::vector<IRInstruction> IRGenerator::getIR() {
    return instructions;
}
