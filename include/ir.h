#ifndef IR_H
#define IR_H
#include "ast.h"

#include "parser.h"
#include <string>
#include <vector>

struct IRInstruction {
    std::string op;                // e.g., "ADD", "MOV", "PRINT"
    std::string arg1, arg2, result; // operands and result
};

class IRGenerator {
private:
    std::vector<IRInstruction> instructions;

public:
    void generate(ASTNode* root);
    void printIR();
    std::vector<IRInstruction> getIR();
};

#endif
