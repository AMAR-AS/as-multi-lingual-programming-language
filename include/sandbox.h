#pragma once
#include <string>

class Sandbox {
public:
    bool validate(const std::string& code) {
        // Always allow for now
        return true;
    }
};
