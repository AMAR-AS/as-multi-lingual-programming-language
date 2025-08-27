#ifndef CODEGEN_H
#define CODEGEN_H

#include "ir.h"
#include <vector>
#include <string>

class CodeGenerator {
public:
    std::string generate(const std::vector<IRInstruction>& ir);
};

#endif
