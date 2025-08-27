#include "ir.h"
#include <iostream>

void IRGenerator::generate(ASTNode* root) {
    if (!root) return;
    if (root->nodeType == "Identifier") {
        IRInstruction instr = {"LOAD", root->value, "", "t" + std::to_string(instructions.size())};
        instructions.push_back(instr);
    }
    for (auto child : root->children) {
        generate(child);
    }
}

void IRGenerator::printIR() {
    for (auto& instr : instructions) {
        std::cout << instr.op << " " << instr.arg1 
                  << " " << instr.arg2 << " -> " 
                  << instr.result << std::endl;
    }
}

std::vector<IRInstruction> IRGenerator::getIR() {
    return instructions;
}
