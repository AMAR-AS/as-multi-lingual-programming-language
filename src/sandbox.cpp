#include "sandbox.h"
#include <iostream>

bool Sandbox::run(const std::vector<IRInstruction>& ir) {
    for (auto& instr : ir) {
        // Safety checks
        if (instr.op == "DIV" && instr.arg2 == "0") {
            std::cerr << "[Sandbox Error] Division by zero detected!\n";
            return false;
        }
        if (instr.op == "MOV" && instr.arg1.empty()) {
            std::cerr << "[Sandbox Error] Invalid MOV instruction!\n";
            return false;
        }
    }

    std::cout << "[Sandbox] All instructions verified and safe.\n";
    return true;
}
