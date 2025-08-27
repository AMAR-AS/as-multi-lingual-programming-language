#ifndef SANDBOX_H
#define SANDBOX_H

#include "ir.h"
#include <vector>

class Sandbox {
public:
    bool run(const std::vector<IRInstruction>& ir);
};

#endif
