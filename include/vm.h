#pragma once
#include <vector>

class VM {
public:
    void load(const std::vector<int>& code);
    int run();
private:
    std::vector<int> program;
};
