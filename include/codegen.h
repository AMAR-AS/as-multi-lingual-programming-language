#pragma once
#include "ir.h"
#include <vector>

class CodeGenerator {
public:
    std::vector<int> generate(const std::vector<IRInstruction>& ir);
};
