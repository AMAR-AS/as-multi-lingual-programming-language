#include "codegen.h"

std::vector<int> CodeGenerator::generate(const std::vector<IRInstruction>& ir) {
    std::vector<int> bytecode;
    bytecode.push_back(255); // HALT
    return bytecode;
}
void Codegen::visitPrint(ASTPrint* node) {
    // First, generate code for the expression (value to print)
    node->expr->accept(*this);

    // Then emit a print instruction
    emit(OP_PRINT);
}
