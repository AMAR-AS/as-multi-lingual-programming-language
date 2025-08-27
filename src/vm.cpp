#include "vm.h"
#include <iostream>

void VM::load(const std::vector<int>& code) {
    program = code;
}

int VM::run() {
    std::cout << "[VM] Running program with " << program.size() << " instructions" << std::endl;
    return 0;
}
case OP_PRINT: {
    if (stack.empty()) {
        std::cerr << "[VM ERROR] PRINT called on empty stack\n";
        running = false;
        break;
    }
    Value v = stack.back();
    stack.pop_back();

    // Simple printing (extend later for lists/maps/etc.)
    if (std::holds_alternative<int>(v)) {
        std::cout << std::get<int>(v);
    } else if (std::holds_alternative<double>(v)) {
        std::cout << std::get<double>(v);
    } else if (std::holds_alternative<std::string>(v)) {
        std::cout << std::get<std::string>(v);
    } else {
        std::cout << "<unknown>";
    }

    std::cout << std::endl;
    break;
}
