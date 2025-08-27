#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "ir.h"
#include <vector>

class Optimizer {
public:
    std::vector<IRInstruction> optimize(const std::vector<IRInstruction>& ir);
};

#endif
