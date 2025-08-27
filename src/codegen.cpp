#include "codegen.h"
#include <sstream>
#include <iostream>

std::string CodeGenerator::generate(const std::vector<IRInstruction>& ir) {
    std::ostringstream asmCode;

    asmCode << "section .text\n";
    asmCode << "global _start\n\n";
    asmCode << "_start:\n";

    for (auto& instr : ir) {
        if (instr.op == "ADD")
            asmCode << "    add " << instr.arg1 << ", " << instr.arg2 << "\n";
        else if (instr.op == "SUB")
            asmCode << "    sub " << instr.arg1 << ", " << instr.arg2 << "\n";
        else if (instr.op == "MOV")
            asmCode << "    mov " << instr.arg1 << ", " << instr.arg2 << "\n";
        else if (instr.op == "PRINT")
            asmCode << "    ; printing not yet implemented in raw asm\n";
    }

    asmCode << "    mov eax, 60   ; syscall: exit\n";
    asmCode << "    xor edi, edi  ; status 0\n";
    asmCode << "    syscall\n";

    return asmCode.str();
}
