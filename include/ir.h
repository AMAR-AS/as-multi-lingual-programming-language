#ifndef IR_H
#define IR_H

#include <string>
#include <vector>
#include "ast.h"

// -------------------------------------
// IR Instruction Structure
// -------------------------------------
struct IRInstruction {
    std::string op;       // operation (MOV, ADD, SUB, etc.)
    std::string arg1;     // first operand
    std::string arg2;     // second operand
    std::string result;   // destination / result
};

// -------------------------------------
// IR Generator
// -------------------------------------
class IRGenerator {
private:
    std::vector<IRInstruction> instructions;
    int tempCounter = 0;  // for unique temporaries

public:
    void generate(ASTNode* root);
    void printIR();
    std::vector<IRInstruction> getIR();
};

#endif // IR_H
