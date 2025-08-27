#include "optimizer.h"
#include <iostream>

std::vector<IRInstruction> Optimizer::optimize(const std::vector<IRInstruction>& ir) {
    std::vector<IRInstruction> optimized = ir;

    for (auto& instr : optimized) {
        // Constant Folding
        if (instr.op == "ADD" || instr.op == "SUB" || instr.op == "MUL" || instr.op == "DIV") {
            if (isdigit(instr.arg1[0]) && isdigit(instr.arg2[0])) {
                int a = std::stoi(instr.arg1);
                int b = std::stoi(instr.arg2);
                int result = 0;

                if (instr.op == "ADD") result = a + b;
                else if (instr.op == "SUB") result = a - b;
                else if (instr.op == "MUL") result = a * b;
                else if (instr.op == "DIV" && b != 0) result = a / b;

                instr.op = "MOV";
                instr.arg1 = std::to_string(result);
                instr.arg2 = "";
            }
        }
    }

    return optimized;
}
