#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "ir.h"
#include "optimizer.h"
#include "codegen.h"
#include "vm.h"
#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: ask <source>" << std::endl;
        return 1;
    }

    std::string source = argv[1];
    Lexer lexer;
    auto tokens = lexer.tokenize(source);

    Parser parser;
    ASTNode* ast = parser.parse(tokens);

    SemanticAnalyzer sema;
    sema.analyze(ast);

    IRGenerator irgen;
    irgen.generate(ast);
    irgen.printIR();

    auto ir = irgen.getIR();
    Optimizer opt;
    opt.optimize(ir);

    CodeGenerator codegen;
    auto bytecode = codegen.generate(ir);

    VM vm;
    vm.load(bytecode);
    return vm.run();
}
