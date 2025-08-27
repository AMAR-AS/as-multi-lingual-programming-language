#pragma once
#include <string>
#include <vector>
#include "ast.h"

struct IRInstruction {
    std::string op;
    std::string arg1;
    std::string arg2;
    std::string result;
};

class IRGenerator {
public:
    void generate(ASTNode* root);
    void printIR();
    std::vector<IRInstruction> getIR();
private:
    std::vector<IRInstruction> instructions;
};
